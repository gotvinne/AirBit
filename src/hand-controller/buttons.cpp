#include "handController.h"
#include <MicroBit.h>

// Button variables:
int throttle = 0;
bool armed = false;

const int THROTTLE_THRESHOLD = 40;
const int THROTTLE_MIN = 5;

static void onButtonB(MicroBitEvent) {
  if (armed) {
    throttle =
        (throttle >= THROTTLE_THRESHOLD) ? (throttle + 1) : (throttle + 5);
  }
  // uBit.display.print(throttle);
}

static void onButtonA(MicroBitEvent) {
  if (armed) {
    throttle = (throttle >= THROTTLE_MIN) ? (throttle - 5) : 0;
  }
  // uBit.display.print(throttle);
}

static void onButtonAB(MicroBitEvent) {
  armed = !armed;
  if (!armed) {
    throttle = 0; // Reset throttle
  }
  // uBit.display.print(armed ? "1" : "0");
}

void setBtnsHandlers() {
  uBit.messageBus.listen(DEVICE_ID_BUTTON_A, DEVICE_BUTTON_EVT_CLICK,
                         onButtonA);
  uBit.messageBus.listen(DEVICE_ID_BUTTON_B, DEVICE_BUTTON_EVT_CLICK,
                         onButtonB);
  uBit.messageBus.listen(DEVICE_ID_BUTTON_AB, DEVICE_BUTTON_EVT_CLICK,
                         onButtonAB);
}

void checkPanic() {
  if (uBit.accelerometer.getGesture() == ACCELEROMETER_EVT_SHAKE) {
    armed = false;
    throttle = 0;
  }
}
