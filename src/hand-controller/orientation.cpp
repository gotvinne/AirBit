#include "orientation.h"
#include "../utilities.h"
#include "handController.h"
#include <MicroBit.h>

// orientation variables:
static int pitch = 0;
static int roll = 0;

const int ANGULAR_THRESHOLD = 45;
const int STEADY_STATE_THRESHOLD = 15;

static int clampAngles(int value, int threshold) {
  int clampedValue = Clamp(value, -threshold, threshold);

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