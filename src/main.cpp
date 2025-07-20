#include "hand-controller/handController.h"
#include <MicroBit.h>

MicroBit uBit;

int main() {
  uBit.init();

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
  return 0;
}