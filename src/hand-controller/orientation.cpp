#include "orientation.h"
#include "handController.h"
#include <MicroBit.h>

// orientation variables:
static int pitch = 0;
static int roll = 0;

const int ANGULAR_THRESHOLD = 45;
const int STEADY_STATE_THRESHOLD = 15;
/*
 * Clamp the variable inside interval
 */
static int clampThreshold(int value, int threshold) {
  return std::min(std::max(value, -threshold),
                  threshold); // implementing std::clamp
}

static int clampAngles(int value, int threshold) {
  int clampedValue = clampThreshold(value, threshold);

  // Set steady state reference:
  if (clampedValue > -STEADY_STATE_THRESHOLD &&
      clampedValue < STEADY_STATE_THRESHOLD) {
    return 0;
  }
  return clampedValue;
}

void SetOrientation() {
  int measuredPitch = uBit.accelerometer.getPitch();
  pitch = clampAngles(measuredPitch, ANGULAR_THRESHOLD);

  int measuredRoll = uBit.accelerometer.getRoll();
  roll = clampAngles(measuredRoll, ANGULAR_THRESHOLD);

  // uBit.display.scroll(pitch);
  // uBit.display.scroll(roll);
}

int GetPitch() { return pitch; }

int GetRoll() { return roll; }