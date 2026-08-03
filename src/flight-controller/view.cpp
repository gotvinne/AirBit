#include "view.h"
#include "../utilities.h"
#include "flightController.h"
#include "receiver.h"
#include <MicroBit.h>
#include <cmath>

// MicroBit display
const uint16_t LED_DISPLAY_SIZE = 5;
const int LED_ON = 255;
const int CHARGING_COLUMN = 4;
const Image ICON_BATTERY_NEEDS_CHARGING =
    Image("0,255,255,255,0\n255,0,255,0,255\n255,255,255,255,255\n0,255,0,255,"
          "0\n0,255,0,255,0\n");
const Image ICON_BATTERY_CHARGING =
    Image("0,0,255,0,0\n0,255,255,255,0\n0,255,255,255,0\n0,255,255,255,0\n0,"
          "255,255,255,0\n");

static void addChargingColumn(Image &ledDisplay) {
  int y = static_cast<int>(GetBatteryState().batteryLevel);
  for (int i = 4; i >= y; i--) {
    ledDisplay.setPixelValue(CHARGING_COLUMN, i, LED_ON);
  }
}

void UpdateViewIdle() {
  uBit.display.clear();

  Image ledDisplay = Image(LED_DISPLAY_SIZE, LED_DISPLAY_SIZE);
  addChargingColumn(ledDisplay);
  DisplayArmed(IsDroneArmed(), ledDisplay);
  DisplayThrottle(GetDroneThrottle(), ledDisplay);
  DisplayPitchRoll(GetDroneRoll(), GetDronePitch(), ledDisplay);
  DisplayYaw(GetDroneYaw(), ledDisplay);
  uBit.display.print(ledDisplay);
}

void UpdateViewCharging() {
  uBit.display.clear();
  uBit.display.print(ICON_BATTERY_CHARGING);
}

void UpdateViewBatteryLow() {
  uBit.display.clear();
  uBit.display.print(ICON_BATTERY_NEEDS_CHARGING);
}
