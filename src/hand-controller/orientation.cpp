#include "handController.h"
#include <MicroBit.h>
#include <algorithm>

// orientation variables:
int pitch = 0;
int roll = 0;

const int ANGULAR_THRESHOLD = 45;
/*
 * Clamp the variable inside interval
 */
static int clampThreshold(int value, int threshold) {
  return std::min(std::max(value, -threshold),
                  threshold); // implementing std::clamp
}

void setOrientation() {
  int measuredPitch = uBit.accelerometer.getPitch();
  pitch = clampThreshold(measuredPitch, ANGULAR_THRESHOLD);

  int measuredRoll = uBit.accelerometer.getRoll();
  roll = clampThreshold(measuredRoll, ANGULAR_THRESHOLD);

  // uBit.display.scroll(pitch);
  // uBit.display.scroll(roll);
}