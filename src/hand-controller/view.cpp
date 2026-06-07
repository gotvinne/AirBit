#include "view.h"
#include "buttons.h"
#include "handController.h"
#include "orientation.h"
#include "pins.h"
#include <MicroBit.h>

// MicroBit display
const uint16_t LED_DISPLAY_SIZE = 5;
const int LED_ON = 255;
int pulseDirection = 15;
int armedPulse = 0;

static void updatePulse() {
  armedPulse += pulseDirection;

  if (armedPulse == 255) {
    pulseDirection = -15;
  } else if (armedPulse == 0) {
    pulseDirection = 15;
  }
}

static void displayArmed(Image &ledDisplay) {
  if (IsArmed()) {
    updatePulse();
    ledDisplay.setPixelValue(0, 0, armedPulse);
  } else {
    armedPulse = 0;
  }
}

static void displayThrottle(Image &ledDisplay) {
  int throttle = GetThrottle();
  if (throttle == 0) {
    return;
  } else {
    int y = floor(4 - (throttle / 25));
    ledDisplay.setPixelValue(0, y, LED_ON);
  }
}

static uint16_t angleToDisplayCoord(int angle) {
  return static_cast<uint16_t>(std::round((angle + 45) / 22.5));
}

static void displayPitchRoll(Image &ledDisplay) {
  uint16_t x = angleToDisplayCoord(GetRoll());
  uint16_t y = angleToDisplayCoord(GetPitch());
  ledDisplay.setPixelValue(x, y, LED_ON);
}

static void displayYaw(Image &ledDisplay) {
  uint16_t x = static_cast<uint16_t>(GetYaw() / 30 + 2);
  ledDisplay.setPixelValue(x, 0, LED_ON);
}

void UpdateDisplay() {
  uBit.display.clear();

  Image ledDisplay = Image(LED_DISPLAY_SIZE, LED_DISPLAY_SIZE);
  displayArmed(ledDisplay);
  displayThrottle(ledDisplay);
  displayPitchRoll(ledDisplay);
  displayYaw(ledDisplay);
  uBit.display.print(ledDisplay);
}
