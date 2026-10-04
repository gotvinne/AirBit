#include "imu.h"
#include "../flightController.h"
#include "I2C.h"

constexpr uint16_t IMU_REG_ADDR = 208; // (104 << 1) 8-bit SLAVE ADDRESS

// IMU register addresses:
constexpr uint8_t WHO_AM_I = 0x75;
constexpr uint8_t PWR_MGMT_1 = 0x6B;
constexpr uint8_t PWR_MGMT_1_RESET = 0x80;
constexpr uint8_t PWR_MGMT_1_WAKE = 0x01;
constexpr uint8_t SAMPLE_RATE_REG = 0x19;
constexpr uint8_t GYRO_CONFIG = 0x1B;
constexpr uint8_t ACCEL_CONFIG = 0x1C;
constexpr uint8_t ACCEL_LOW_PASS_FILTER = 0x1D;
constexpr uint8_t GYRO_LOW_PASS_FILTER = 0x1A;

// Register values:
constexpr uint8_t GYRO_XOUT = 0x43; // 67
constexpr uint8_t GYRO_YOUT = 0x45; // 69
constexpr uint8_t GYRO_ZOUT = 0x47; // 71
constexpr uint8_t ACCEL_XOUT = 0x3D;
constexpr uint8_t ACCEL_YOUT = 0x3B;
constexpr uint8_t ACCEL_ZOUT = 0x3F;
constexpr uint8_t GYRO_CONFIG_VALUE =
    0x18; // sets gyro full scale range to ±2000°/s
constexpr uint8_t GYRO_LOW_PASS_FILTER_VALUE = 0x05;
constexpr uint8_t ACCEL_LOW_PASS_FILTER_VALUE = 0x06;

constexpr float GYRO_SCALE_2000_DPS =
    16.4f; // LSB scale factor for ±2000°/s full scale range
constexpr float ACCEL_SCALE_2G =
    16384.0f; // This is the number of counts per g.
constexpr float RAD_TO_DEG = 180.0f / 3.141592653589793f;
constexpr int IMU_CALIBRATION_SAMPLES = 100;

// Complementary filter
constexpr float ALPHA = 0.98f; // Complementary filter coefficient
static float rollAcc = 0.0f;
static float pitchAcc = 0.0f;
static int lastTimeMs = 0;

static IMUState imuState{
  isAvailable : false,
  imu : {0, 0, 0, 0, 0},
  offsets : {0, 0, 0, 0, 0, 0},
};

static bool writeRegister(uint8_t reg, uint8_t value) {
  return uBit.i2c.writeRegister(IMU_REG_ADDR, reg, value) != 0;
}

static bool readRegister(uint8_t reg, uint8_t &value) {
  return uBit.i2c.readRegister(IMU_REG_ADDR, reg, &value, 1) != 0;
}

static int16_t readImuReg(uint8_t reg) {
  uint8_t data[2] = {0, 0};
  if (uBit.i2c.readRegister(IMU_REG_ADDR, reg, data, 2) != 0) {
    return 0;
  }

  return static_cast<int16_t>((static_cast<uint16_t>(data[0]) << 8) |
                              static_cast<uint16_t>(data[1]));
}

static void calibrateImuOffsets() {
  int gyroXSum = 0;
  int gyroYSum = 0;
  int gyroZSum = 0;
  int accXCounts = 0;
  int accYCounts = 0;
  int accZCounts = 0;

  for (int i = 0; i < IMU_CALIBRATION_SAMPLES; i++) {
    gyroXSum += readImuReg(GYRO_XOUT);
    gyroYSum += readImuReg(GYRO_YOUT);
    gyroZSum += readImuReg(GYRO_ZOUT);
    accXCounts += readImuReg(ACCEL_XOUT);
    accYCounts += readImuReg(ACCEL_YOUT);
    accZCounts += readImuReg(ACCEL_ZOUT);
    uBit.sleep(5);
  }

  imuState.offsets.gyroX = gyroXSum / IMU_CALIBRATION_SAMPLES;
  imuState.offsets.gyroY = gyroYSum / IMU_CALIBRATION_SAMPLES;
  imuState.offsets.gyroZ = gyroZSum / IMU_CALIBRATION_SAMPLES;
  imuState.offsets.accX = accXCounts / IMU_CALIBRATION_SAMPLES;
  imuState.offsets.accY = accYCounts / IMU_CALIBRATION_SAMPLES;
  imuState.offsets.accZ =
      (accZCounts / IMU_CALIBRATION_SAMPLES) - static_cast<int>(ACCEL_SCALE_2G);
}

static void updateGyro() {
  const float rawPitchDps =
      static_cast<float>(readImuReg(GYRO_XOUT) - imuState.offsets.gyroX);
  const float rawRollDps =
      static_cast<float>(readImuReg(GYRO_YOUT) - imuState.offsets.gyroY);
  const float rawYawDps =
      static_cast<float>(readImuReg(GYRO_ZOUT) - imuState.offsets.gyroZ);
  imuState.imu.pitchRate = static_cast<int>(rawPitchDps / GYRO_SCALE_2000_DPS);
  imuState.imu.rollRate = static_cast<int>(rawRollDps / GYRO_SCALE_2000_DPS);
  imuState.imu.yawRate = static_cast<int>(rawYawDps / GYRO_SCALE_2000_DPS);

  // uBit.serial.printf("%spitchRate=%d\x1b[0m ",
  // imuState.imu.pitchRate < 0 ? "\x1b[31m" : "\x1b[32m",
  // imuState.imu.pitchRate);
  // uBit.serial.printf("%srollRate=%d\x1b[0m ",
  // imuState.imu.rollRate < 0 ? "\x1b[31m" : "\x1b[32m",
  // imuState.imu.rollRate);
}

static void updateAccelerometer() {
  // ax, ay and az are calculated in g's, where 1g = 9.81 m/s^2.
  // Expected measurements are approximately ax=0, ay=0, az=1g when the drone is
  // level and stationary.
  const float ax = static_cast<float>(
      (readImuReg(ACCEL_XOUT) - imuState.offsets.accX) / ACCEL_SCALE_2G);
  const float ay = static_cast<float>(
      (readImuReg(ACCEL_YOUT) - imuState.offsets.accY) / ACCEL_SCALE_2G);
  const float az = static_cast<float>(
      (readImuReg(ACCEL_ZOUT) - imuState.offsets.accZ) / ACCEL_SCALE_2G);

  rollAcc = atan2(-ay, az) * RAD_TO_DEG;
  pitchAcc = asin(ax) * RAD_TO_DEG; // ax is measured in g.

  // uBit.serial.printf("%srollAcc=%d\x1b[0m ",
  // rollAcc < 0 ? "\x1b[31m" : "\x1b[32m",
  // static_cast<int>(rollAcc));
  // uBit.serial.printf("%spitchAcc=%d\x1b[0m ",
  // pitchAcc < 0 ? "\x1b[31m" : "\x1b[32m",
  // static_cast<int>(pitchAcc));
}

static void printAccelerometerMeas(float ax, float ay, float az) {
  int ax_mg = static_cast<int>(ax * 1000.0f);
  int ay_mg = static_cast<int>(ay * 1000.0f);
  int az_mg = static_cast<int>(az * 1000.0f);

  uBit.serial.printf("%saxMg=%d\x1b[0m ", ax_mg < 0 ? "\x1b[31m" : "\x1b[32m",
                     ax_mg);
  uBit.serial.printf("%sayMg=%d\x1b[0m ", ay_mg < 0 ? "\x1b[31m" : "\x1b[32m",
                     ay_mg);
  uBit.serial.printf("%sazMg=%d\x1b[0m ", az_mg < 0 ? "\x1b[31m" : "\x1b[32m",
                     az_mg);
}

static void complementaryFilter() {
  int nowMs = uBit.systemTime();

  float dt = 0.0f;
  if (lastTimeMs != 0) {
    dt = (nowMs - lastTimeMs) / 1000.0f; // convert ms -> seconds
  }

  const float roll =
      ALPHA * (imuState.imu.roll + GetRollRate() * dt) + (1 - ALPHA) * rollAcc;
  const float pitch = ALPHA * (imuState.imu.pitch + GetPitchRate() * dt) +
                      (1 - ALPHA) * pitchAcc;

  imuState.imu.roll = static_cast<int>(roll);
  imuState.imu.pitch = static_cast<int>(pitch);
  lastTimeMs = nowMs;
  // uBit.serial.printf("%sroll=%d\x1b[0m ",
  // imuState.imu.roll < 0 ? "\x1b[31m" : "\x1b[32m",
  // imuState.imu.roll);
  // uBit.serial.printf("%spitch=%d\x1b[0m ",
  // imuState.imu.pitch < 0 ? "\x1b[31m" : "\x1b[32m",
  // imuState.imu.pitch);
}

void InitIMU() {
  if (writeRegister(PWR_MGMT_1, PWR_MGMT_1_RESET)) {
    SetState(State::ERROR);
    SetErrorMessage("IMU 0");
    return;
  }

  uBit.sleep(100);
  uint8_t whoami = 0;
  readRegister(WHO_AM_I, whoami);

  if (whoami == 0) {
    SetState(State::ERROR);
    SetErrorMessage("IMU 1");
    return;
  }

  if (writeRegister(PWR_MGMT_1, PWR_MGMT_1_WAKE) ||
      writeRegister(SAMPLE_RATE_REG, 0x07) ||
      writeRegister(GYRO_LOW_PASS_FILTER, GYRO_LOW_PASS_FILTER_VALUE) ||
      writeRegister(GYRO_CONFIG, GYRO_CONFIG_VALUE) ||
      writeRegister(ACCEL_CONFIG, 0x00) ||
      writeRegister(ACCEL_LOW_PASS_FILTER, ACCEL_LOW_PASS_FILTER_VALUE)) {
    SetState(State::ERROR);
    SetErrorMessage("IMU 2");
    return;
  }

  calibrateImuOffsets();
  imuState.isAvailable = true;
}

void UpdateIMUMeasurements() {
  if (!imuState.isAvailable) {
    return;
  }
  updateGyro();
  updateAccelerometer();
  complementaryFilter();
}

int GetRollRate() { return imuState.imu.rollRate; }
int GetPitchRate() { return imuState.imu.pitchRate; }
int GetYawRate() { return imuState.imu.yawRate; }
int GetComplementaryRoll() { return imuState.imu.roll; }
int GetComplementaryPitch() { return imuState.imu.pitch; }

bool IsIMUAvailable() { return imuState.isAvailable; }
