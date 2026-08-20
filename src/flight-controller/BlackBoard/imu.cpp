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
constexpr uint8_t ACCEL_XOUT = 0x3B;
constexpr uint8_t ACCEL_YOUT = 0x3D;
constexpr uint8_t ACCEL_ZOUT = 0x3F;
constexpr uint8_t GYRO_CONFIG_VALUE =
    0x18; // sets gyro full scale range to ±2000°/s
constexpr uint8_t GYRO_LOW_PASS_FILTER_VALUE = 0x05;
constexpr uint8_t ACCEL_LOW_PASS_FILTER_VALUE = 0x06;

constexpr float GYRO_SCALE_2000_DPS =
    16.4f; // LSB scale factor for ±2000°/s full scale range
constexpr float ACCEL_SCALE_2G = 16384.0f; // ±2g
constexpr float RAD_TO_DEG = 180.0f / 3.141592653589793f;
constexpr int IMU_CALIBRATION_SAMPLES = 100;

static IMUState imuState{
  isAvailable : false,
  imu : {0, 0, 0, 0, 0},
  offsets : {0, 0, 0, 0, 0},
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
  int rollDpsSum = 0;
  int pitchDpsSum = 0;
  int yawDpsSum = 0;
  int rollSum = 0;
  int pitchSum = 0;

  for (int i = 0; i < IMU_CALIBRATION_SAMPLES; i++) {
    rollDpsSum += readImuReg(GYRO_XOUT);
    pitchDpsSum += readImuReg(GYRO_YOUT);
    yawDpsSum += readImuReg(GYRO_ZOUT);
    rollSum += readImuReg(ACCEL_XOUT);
    pitchSum += readImuReg(ACCEL_YOUT);
    uBit.sleep(5);
  }

  imuState.offsets.gyroX = rollDpsSum / IMU_CALIBRATION_SAMPLES;
  imuState.offsets.gyroY = pitchDpsSum / IMU_CALIBRATION_SAMPLES;
  imuState.offsets.gyroZ = yawDpsSum / IMU_CALIBRATION_SAMPLES;
  imuState.offsets.accX = rollSum / IMU_CALIBRATION_SAMPLES;
  imuState.offsets.accY = pitchSum / IMU_CALIBRATION_SAMPLES;
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

  const float rawPitchDps =
      static_cast<float>(readImuReg(GYRO_XOUT) - imuState.offsets.gyroX);
  const float rawRollDps =
      static_cast<float>(readImuReg(GYRO_YOUT) - imuState.offsets.gyroY);
  const float rawYawDps =
      static_cast<float>(readImuReg(GYRO_ZOUT) - imuState.offsets.gyroZ);
  const float ax =
      static_cast<float>(readImuReg(ACCEL_XOUT) - imuState.offsets.accX) /
      ACCEL_SCALE_2G;
  const float ay =
      static_cast<float>(readImuReg(ACCEL_YOUT) - imuState.offsets.accY) /
      ACCEL_SCALE_2G;
  const float az = static_cast<float>(readImuReg(ACCEL_ZOUT)) / ACCEL_SCALE_2G;

  imuState.imu.pitchRate = static_cast<int>(rawPitchDps / GYRO_SCALE_2000_DPS);
  imuState.imu.rollRate = static_cast<int>(rawRollDps / GYRO_SCALE_2000_DPS);
  imuState.imu.yawRate = static_cast<int>(rawYawDps / GYRO_SCALE_2000_DPS);

  const float roll = atan2(ay, sqrt(ax * ax + az * az)) * RAD_TO_DEG;
  const float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * RAD_TO_DEG;

  imuState.imu.roll = static_cast<int>(roll);
  imuState.imu.pitch = static_cast<int>(pitch);

  // uBit.serial.printf("rollDps=%d pitchDps=%d yawDps=%d\t",
  //                imuState.imu.rollRate,
  //            imuState.imu.pitchRate,
  //        imuState.imu.yawRate);
  // uBit.serial.printf("roll=%d pitch=%d\t", imuState.imu.roll,
  // imuState.imu.pitch);
}

int GetRollRate() { return imuState.imu.rollRate; }
int GetPitchRate() { return imuState.imu.pitchRate; }
int GetYawRate() { return imuState.imu.yawRate; }

bool IsIMUAvailable() { return imuState.isAvailable; }
