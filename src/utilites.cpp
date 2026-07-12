#include "utilities.h"

static int pulseDirection = 15;
static int armedPulse = 0;

static void updatePulse() {
  armedPulse += pulseDirection;

  if (armedPulse == 255) {
    pulseDirection = -15;
  } else if (armedPulse == 0) {
    pulseDirection = 15;
  }
}

void DisplayArmed(bool isArmed, Image &ledDisplay) {
  if (isArmed) {
    updatePulse();
    ledDisplay.setPixelValue(0, 0, armedPulse);
  } else {
    armedPulse = 0;
  }
}