#include "handController.h"

void InitHandController() {
  SetBtnsHandlers();
  SetP1High();
  CalibrateHandAccelerometer();
}

void HandController() {
  InitHandController();

  while (true) {
    SetOrientation();
    UpdateDisplay();
    CheckPanic();
    TransmittData();
    ReadPins();

    uBit.sleep(100);
  }
}