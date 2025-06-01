#include "hand-controller/handController.h"
#include <MicroBit.h>

MicroBit uBit;

int main() {
  uBit.init();

  setBtnsHandlers();

  while (true) {
    checkPanic();
    setOrientation();
    updateDisplay();
    uBit.sleep(50); // Do not use 100% cpu
  }
  return 0;
}