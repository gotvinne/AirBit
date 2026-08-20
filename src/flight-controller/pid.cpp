#include "pid.h"
#include "BlackBoard/imu.h"
#include "flightController.h"

const uint8_t ARMED_THROTTLE = 5;
constexpr int CALIBRATION_SAMPLES = 100;

// PID controller parameters
const float KP_PITCH_ROLL = 0.9;
const float KD_PITCH_ROLL = 15;
const float KI_PITCH_ROLL = 0.004;
const float KP_YAW = 5;
const float KD_YAW = 70;

// Orientation variables:
static int pitchOffset = 0;
static int rollOffset = 0;

// Time tracking variables for PID controller
static float epSum = 0;
static float erSum = 0;

static PIDOutput PIDCalculate() {
  int measuredPitch = uBit.accelerometer.getPitch() - pitchOffset;
  int measuredRoll = uBit.accelerometer.getRoll() - rollOffset;
  uBit.serial.printf("roll: %d, pitch: %d\t", measuredRoll, measuredPitch);

  int ep = GetDronePitch() - measuredPitch;
  int er = GetDroneRoll() - measuredRoll;
  epSum += ep;
  erSum += er;

  // u = Kp * ex + Ki * integral(ex) + Kd * derivative(ex)
  const int pitchCmd =
      static_cast<int>(KP_PITCH_ROLL * ep + KI_PITCH_ROLL * epSum +
                       KD_PITCH_ROLL * GetPitchRate());
  const int rollCmd =
      static_cast<int>(KP_PITCH_ROLL * er + KI_PITCH_ROLL * erSum +
                       KD_PITCH_ROLL * GetRollRate());

  return {0, 0};
}

void CalibrateDroneAccelerometer() {
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

void SetPIDActuation() {
  uint8_t throttle = GetDroneThrottle() * 2.55;
  if (throttle == 0) {
    UpdatePropellerActuationEqual(ARMED_THROTTLE);
  } else {
    PIDOutput pidOutput = PIDCalculate();
    MotorMixing(throttle, 0, pidOutput.pitch, pidOutput.roll);
  }
}
