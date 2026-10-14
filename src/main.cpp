#include <MicroBit.h>

#ifndef AIRBIT_CONTROLLER
#error "AIRBIT_CONTROLLER must be defined in codal.json"
#endif

#if AIRBIT_CONTROLLER == 0
#include "flight-controller/flightController.h"
#elif AIRBIT_CONTROLLER == 1
#include "hand-controller/handController.h"
#else
#error "Invalid AIRBIT_CONTROLLER: expected 0 (flight) or 1 (hand)"
#endif

MicroBit uBit;
const uint8_t RADIO_GROUP = 7;

static void initRadio() {
  uBit.radio.enable();
  uBit.radio.setGroup(RADIO_GROUP);
  uBit.display.print(RADIO_GROUP);
  uBit.sleep(3000);
  uBit.display.clear();
}

int main() {
  uBit.init();
  initRadio();
#if AIRBIT_CONTROLLER == 0
  FlightController();
#elif AIRBIT_CONTROLLER == 1
  HandController();
#endif
  return 0;
}