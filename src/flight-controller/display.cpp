#include "flightController.h"
#include <MicroBit.h>
#include <cmath>

// MicroBit display
const uint16_t LED_DISPLAY_SIZE = 5;
const int LED_ON = 255;
const Image ICON_BATTERY_NEEDS_CHARGING =
    Image("0,255,255,255,0\n255,0,255,0,255\n255,255,255,255,255\n0,255,0,255,"
          "0\n0,255,0,255,0\n");
const Image ICON_BATTERY_CHARGING =
    Image("0,0,255,0,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n0,"
          "255,255,255,0\n");

static void addChargingColumn(int column, Image &ledDisplay) {
  if (column < 0 || column >= LED_DISPLAY_SIZE) {
    return;
  }
  int y = static_cast<int>(batteryLevel);
  for (int i = 4; i >= y; i--) {
    ledDisplay.setPixelValue(column, i, LED_ON);
  }
}

static void displayBatteryLevel(Image &ledDisplay) {
  if (isCharging) {
    ledDisplay = ICON_BATTERY_CHARGING;
    return;
  }

  if (batteryLevel == BatteryLevel::EMPTY ||
      batteryLevel == BatteryLevel::LOW) {
    ledDisplay = ICON_BATTERY_NEEDS_CHARGING;
    return;
  }

  addChargingColumn(4, ledDisplay);
}

void UpdateDisplay() {
  uBit.display.clear();

  Image ledDisplay = Image(LED_DISPLAY_SIZE, LED_DISPLAY_SIZE);
  displayBatteryLevel(ledDisplay);
  uBit.display.print(ledDisplay);
}
