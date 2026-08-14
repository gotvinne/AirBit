#include "gyro.h"
#include "../flightController.h"
#include "I2C.h"

constexpr uint16_t IMU_REG_ADDR = 208; // (104 << 1) 8-bit SLAVE ADDRESS

// IMU register addresses:
constexpr uint8_t WHO_AM_I = 0x75;
constexpr uint8_t PWR_MGMT_1 = 0x6B;
constexpr uint8_t PWR_MGMT_1_RESET = 0x80;
constexpr uint8_t PWR_MGMT_1_WAKE = 0x01;
constexpr uint8_t SAMPLE_RATE_REG = 0x19;
constexpr uint8_t LOW_PASS_FILTER = 0x1A;
constexpr uint8_t GYRO_CONFIG = 0x1B;
constexpr uint8_t GYRO_CONFIG_VALUE =
    0x18; // sets gyro full scale range to ±2000°/s
constexpr uint8_t ACCEL_CONFIG = 0x1C;

constexpr uint8_t GYRO_XOUT = 0x43; // 67
constexpr uint8_t GYRO_YOUT = 0x45; // 69
constexpr uint8_t GYRO_ZOUT = 0x47; // 71
constexpr float GYRO_SCALE_2000_DPS =
    16.4f; // LSB scale factor for ±2000°/s full scale range
constexpr int GYRO_CALIBRATION_SAMPLES = 100;

static GyroState gyroState{
  isAvailable : false,
  rollOffset : 0,
  pitchOffset : 0,
  yawOffset : 0,
  rollRate : 0,
  pitchRate : 0,
  yawRate : 0
};

static bool writeRegister(uint8_t reg, uint8_t value) {
  return uBit.i2c.writeRegister(IMU_REG_ADDR, reg, value) != 0;
}

static bool readRegister(uint8_t reg, uint8_t &value) {
  return uBit.i2c.readRegister(IMU_REG_ADDR, reg, &value, 1) != 0;
}

static int16_t readGyroReg(uint8_t reg) {
  uint8_t data[2] = {0, 0};
  if (uBit.i2c.readRegister(IMU_REG_ADDR, reg, data, 2) != 0) {
    return 0;
  }

  return static_cast<int16_t>((static_cast<uint16_t>(data[0]) << 8) |
                              static_cast<uint16_t>(data[1]));
}

static void calibrateGyroOffsets() {
  int rollSum = 0;
  int pitchSum = 0;
  int yawSum = 0;

  for (int i = 0; i <= GYRO_CALIBRATION_SAMPLES; i++) {
    rollSum += readGyroReg(GYRO_XOUT);
    pitchSum += readGyroReg(GYRO_YOUT);
    yawSum += readGyroReg(GYRO_ZOUT);
    uBit.sleep(5);
  }

  gyroState.rollOffset = rollSum / GYRO_CALIBRATION_SAMPLES;
  gyroState.pitchOffset = pitchSum / GYRO_CALIBRATION_SAMPLES;
  gyroState.yawOffset = yawSum / GYRO_CALIBRATION_SAMPLES;
}

void InitGyro() {
  if (writeRegister(PWR_MGMT_1, PWR_MGMT_1_RESET)) {
    SetState(State::ERROR);
    SetErrorMessage("GYRO 0");
    return;
  }

  uBit.sleep(100);
  uint8_t whoami = 0;
  readRegister(WHO_AM_I, whoami);

  if (whoami == 0) {
    SetState(State::ERROR);
    SetErrorMessage("GYRO 1");
    return;
  }

  if (writeRegister(PWR_MGMT_1, PWR_MGMT_1_WAKE) ||
      writeRegister(SAMPLE_RATE_REG, 0x07) ||
      writeRegister(LOW_PASS_FILTER, 0x00) ||
      writeRegister(GYRO_CONFIG, GYRO_CONFIG_VALUE) ||
      writeRegister(ACCEL_CONFIG, 0x00)) {
    SetState(State::ERROR);
    SetErrorMessage("GYRO 2");
    return;
  }

  calibrateGyroOffsets();
  gyroState.isAvailable = true;
}

void UpdateGyroMeasurements() {
  if (!gyroState.isAvailable) {
    return;
  }

  const float rawPitch =
      static_cast<float>(readGyroReg(GYRO_XOUT) - gyroState.pitchOffset);
  const float rawRoll =
      static_cast<float>(readGyroReg(GYRO_YOUT) - gyroState.rollOffset);
  const float rawYaw =
      static_cast<float>(readGyroReg(GYRO_ZOUT) - gyroState.yawOffset);

  gyroState.pitchRate = static_cast<int>(rawPitch / GYRO_SCALE_2000_DPS);
  gyroState.rollRate = static_cast<int>(rawRoll / GYRO_SCALE_2000_DPS);
  gyroState.yawRate = static_cast<int>(rawYaw / GYRO_SCALE_2000_DPS);

  // uBit.serial.printf("rollDps=%d pitchDps=%d yawDps=%d\n",
  //                 gyroState.rollRate,
  //               gyroState.pitchRate,
  //             gyroState.yawRate);
}

const int &GetRollRate() { return gyroState.rollRate; }
const int &GetPitchRate() { return gyroState.pitchRate; }
const int &GetYawRate() { return gyroState.yawRate; }

bool IsGyroAvailable() { return gyroState.isAvailable; }
