#include "pid.h"
#include "BlackBoard/gyro.h"
#include "flightController.h"

const uint8_t ARMED_THROTTLE = 5;
constexpr int CALIBRATION_SAMPLES = 100;

// PID controller parameters
const int KP_PITCH_ROLL = 1;
const int KD_PITCH_ROLL = 0;
const int KI_PITCH_ROLL = 1;
const int KP_YAW = 1;
const int KD_YAW = 0;

// orientation variables:
static int pitchOffset = 0;
static int rollOffset = 0;

static int deltaTime = 0;
static unsigned long lastTime = 0;

void CalibrateAccelerometer() {
  int rollSum = 0;
  int pitchSum = 0;

  for (int i = 0; i <= CALIBRATION_SAMPLES; i++) {
    rollSum += uBit.accelerometer.getRoll();
    pitchSum += uBit.accelerometer.getPitch();
    uBit.sleep(5);
  }

  rollOffset = rollSum / CALIBRATION_SAMPLES;
  pitchOffset = pitchSum / CALIBRATION_SAMPLES;
}

void UpdateFlightOrientation() {
  int measuredPitch = uBit.accelerometer.getPitch() - pitchOffset;
  int measuredRoll = uBit.accelerometer.getRoll() - rollOffset;
  deltaTime = static_cast<int>(uBit.systemTime() - lastTime);
  lastTime = uBit.systemTime();

  // uBit.serial.printf("roll: %d, pitch: %d\n", measuredRoll, measuredPitch);
}

void SetThrottle() {
  uint8_t throttle = GetDroneThrottle() * 2.55;
  if (throttle == 0) {
    UpdatePropellerActuationEqual(ARMED_THROTTLE);
  } else {
    UpdatePropellerActuationEqual(throttle);
  }
}
