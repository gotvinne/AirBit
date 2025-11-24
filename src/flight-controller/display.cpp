#include "flightController.h"
#include <MicroBit.h>
#include <cmath>

// MicroBit display
const uint16_t LED_DISPLAY_SIZE = 5;
const int LED_ON = 255;

static uint16_t milliVoltToDisplayCoord() {
  return static_cast<uint16_t>(std::round(21 - (batteryMilliVolt / 200)));
}

static void displayBatteryLevel(Image &ledDisplay) {
  uint16_t y = milliVoltToDisplayCoord();
  // Iterating downwards
  for (uint16_t i = 4; i > y; i--) {
    ledDisplay.setPixelValue(4, i, LED_ON);
  }
}

void UpdateDisplay() {
  uBit.display.clear();

  Image ledDisplay = Image(LED_DISPLAY_SIZE, LED_DISPLAY_SIZE);
  displayBatteryLevel(ledDisplay);
  uBit.display.print(ledDisplay);
}
