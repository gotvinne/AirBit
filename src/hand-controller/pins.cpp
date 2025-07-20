#include "handController.h"
#include <MicroBit.h>

int yaw = 0;

// ADC has a resolution of 1 Byte
// There are different thresholds for the two pins, as P0 is more sensible
const int P0_HIGH_ANALOG_THRESHOLD = 850;
const int P0_LOW_ANALOG_THRESHOLD = 600;

const int P2_HIGH_ANALOG_THRESHOLD = 700;
const int P2_LOW_ANALOG_THRESHOLD = 400;

const int MAX_PWM_VALUE = 1023;

static int setYawFromP0Read(int meas) {
    if (meas >= P0_HIGH_ANALOG_THRESHOLD) {
        return -30;
    } else if (meas <= P0_LOW_ANALOG_THRESHOLD) {
        return 0;
    } else { // If measurement is within range
        return yaw;
    }
}

static int setYawFromP2Read(int meas) {
    if (meas >= P2_HIGH_ANALOG_THRESHOLD) {
        return 30;
    } else if (meas <= P2_LOW_ANALOG_THRESHOLD) {
        return 0;
    } else {
        return yaw;
    }
}

void setP1High() {
  int succ = uBit.io.P1.setAnalogValue(MAX_PWM_VALUE);
  if (succ != DEVICE_OK) {
    uBit.display.scroll("P1");
    uBit.sleep(1000);
  }
}

void readPins() {
  int p0 = uBit.io.P0.getAnalogValue();
  int p2 = uBit.io.P2.getAnalogValue();

  // Since the yaw can either be determined of P0 or P2
  // We let the highest measured value determine yaw if it exeeds HIGH_ANALOG_THRESHOLD
  // Reset yaw if the highest measured value is lower than LOW_ANALOG_THRESHOLD
  if (p0 > p2) {
    yaw = setYawFromP0Read(p0);
  } else if (p2 > p0) {
    yaw = setYawFromP2Read(p2);
  } 
}