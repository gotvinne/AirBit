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
  uBit.display.clear();
}

static void FlightController() {
  InitFlightController();
  if (GetFlightState().state == State::ERROR) {
    while (true) {
      uBit.display.scroll(GetFlightState().errorMessage);
      uBit.sleep(1000);
    }
    return;
  }
  SetState(State::IDLE);

  while (true) {
    SetBatteryInfo();
    FlushRadioBuffer();
    CheckFlightState();

    switch (GetFlightState().state) {
    case State::ARMED:
      UpdateViewIdle();
      SetThrottle();
      break;
    case State::IDLE:
      UpdateViewIdle();
      UpdatePropellerActuationEqual(0);
      break;
    case State::LOWBATTERY:
      UpdateViewBatteryLow();
      UpdatePropellerActuationEqual(0);
      break;
    case State::CHARGING:
      UpdateViewCharging();
      UpdatePropellerActuationEqual(0);
      break;
    default:
      UpdatePropellerActuationEqual(0);
      return; // If going to CALIBRATING or ERROR state
    }

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