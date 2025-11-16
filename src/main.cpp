#include "flight-controller/flightController.h"
#include "hand-controller/handController.h"
#include <MicroBit.h>

MicroBit uBit;

static void handController() {
  initRadio();
  setBtnsHandlers();
  setP1High();

  while (true) {
    checkPanic();
    readPins();
    setOrientation();
    updateDisplay();
    transmittData();
    uBit.sleep(100); // Do not use 100% cpu
  }
}

static void ultrasonicSensor() {
  initUltrasonicSensor();

  while (true) { // measure one per second
    startMeasurement();
    uBit.display.scroll(altitude);
    uBit.sleep(1000);
  }
}

int main() {
  uBit.init();

  ultrasonicSensor();
  return 0;
}