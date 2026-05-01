#include "flight-controller/flightController.h"
#include <MicroBit.h>

MicroBit uBit;

int main() {
  uBit.init();

  InitBatteryInfo();
  InitServoController();
  SetAllPropellerActuation(0, 0, 0, 0);
  while (true) {
    SetBatteryInfo();
    UpdateDisplay();
    uBit.sleep(1000);
  }
  return 0;
}