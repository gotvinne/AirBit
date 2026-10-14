#include "handController.h"

void HandController() {
  SetBtnsHandlers();
  SetP1High();
  CalibrateHandAccelerometer();

  while (true) {
    SetOrientation();
    UpdateDisplay();
    CheckPanic();
    TransmittData();
    ReadPins();

    uBit.sleep(100);
  }
}