#include "handController.h"
#include <MicroBit.h>
#include <cmath>

// MicroBit display
const uint16_t LED_DISPLAY_SIZE = 5;
const int LED_ON = 255;

static void displayArmed(Image &ledDisplay) {
  if (armed) {
    ledDisplay.setPixelValue(0, 0, LED_ON);
  }
}

static void displayThrottle(Image &ledDisplay) {
  if (throttle == 0) {
    return;
  } else {
    int y = floor(4 - (throttle / 25));
    ledDisplay.setPixelValue(0, y, LED_ON);
  }
}

static uint16_t angleToDisplayCoord(int angle) {
  return static_cast<uint16_t>((angle + 45) / 22.5);
}

static void displayPitchRoll(Image &ledDisplay) {
  uint16_t x = angleToDisplayCoord(roll);
  uint16_t y = angleToDisplayCoord(pitch);
  ledDisplay.setPixelValue(x, y, LED_ON);
}

void updateDisplay() {
  uBit.display.clear();

  Image ledDisplay = Image(LED_DISPLAY_SIZE, LED_DISPLAY_SIZE);
  displayArmed(ledDisplay);
  displayThrottle(ledDisplay);
  displayPitchRoll(ledDisplay);
  uBit.display.print(ledDisplay);
}
