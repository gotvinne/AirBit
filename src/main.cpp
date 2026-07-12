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
    FlushRadioBuffer();
    UpdateView();
    uBit.sleep(100);
  }
}

static void HandController() {
  SetBtnsHandlers();
  SetP1High();

  while (true) {
    SetOrientation();
    UpdateDisplay();
    CheckPanic();
    TransmittData();
    ReadPins();
    uBit.sleep(100);
  }
}

int main() {
  uBit.init();
  initRadio();
  FlightController();
  return 0;
}