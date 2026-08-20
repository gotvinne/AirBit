#include "orientation.h"
#include "../utilities.h"
#include "handController.h"
#include <MicroBit.h>

// orientation variables:
static int pitch = 0;
static int roll = 0;
static int pitchOffset = 0;
static int rollOffset = 0;

constexpr int CALIBRATION_SAMPLES = 100;
constexpr int ANGULAR_THRESHOLD = 45;
constexpr int STEADY_STATE_THRESHOLD = 10;

static int clampAngles(int value, int threshold) {
  int clampedValue = Clamp(value, -threshold, threshold);

  // Set steady state reference:
  if (clampedValue > STEADY_STATE_THRESHOLD) {
    return clampedValue - STEADY_STATE_THRESHOLD;
  } else if (clampedValue < -STEADY_STATE_THRESHOLD) {
    return clampedValue + STEADY_STATE_THRESHOLD;
  } else {
    return 0;
  }
  return clampedValue;
}

void CalibrateHandAccelerometer() {
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

void SetOrientation() {
  int measuredPitch = uBit.accelerometer.getPitch() - pitchOffset;
  pitch = clampAngles(measuredPitch, ANGULAR_THRESHOLD);

  int measuredRoll = uBit.accelerometer.getRoll() - rollOffset;
  roll = clampAngles(measuredRoll, ANGULAR_THRESHOLD);

  // uBit.serial.printf("roll: %d, pitch: %d\t", roll, pitch);
}

int GetPitch() { return pitch; }

int GetRoll() { return roll; }