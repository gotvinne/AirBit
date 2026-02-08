#include "flight-controller/flightController.h"
#include <MicroBit.h>

MicroBit uBit;

int main() {
  uBit.init();

  InitBatteryInfo();
  while (true) {
    SetBatteryInfo();
    UpdateDisplay();
    uBit.sleep(1000);
  }
  return 0;
}