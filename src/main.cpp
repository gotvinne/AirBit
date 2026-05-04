#include "flight-controller/flightController.h"
#include "hand-controller/handController.h"
#include <MicroBit.h>

MicroBit uBit;
const uint8_t RADIO_GROUP = 7;

static void initRadio() {
  uBit.radio.enable();
  uBit.radio.setGroup(RADIO_GROUP);
  uBit.display.print(RADIO_GROUP);
  uBit.sleep(3000);
}

static void FlightController() {
  SetState(State::CALIBRATING);
  InitBatteryInfo();
  InitServoController();
  if (GetFlightState().state == State::PANIC) {
    uBit.sleep(10000);
    return;
  } else {
    SetState(State::DISARMED);
  }

  while (true) {
    SetBatteryInfo();
    UpdateView();
    uBit.sleep(1000);
  }
}

static void HandController() {
  setBtnsHandlers();
  setP1High();

  while (true) {
    setOrientation();
    updateDisplay();
    checkPanic();
    transmittData();
    readPins();
    uBit.sleep(100);
  }
}

int main() {
  uBit.init();
  initRadio();
  FlightController();
  return 0;
}