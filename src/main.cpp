#include "hand-controller/handController.h"
#include <MicroBit.h>

MicroBit uBit;

int main() {
  uBit.init();

  setBtnsHandlers();

  while (1) {
    setOrientation();
    updateDisplay();

    uBit.sleep(100); // Do not use 100% cpu
  }
  return 0;
}