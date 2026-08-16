#include "handController.h"

void InitHandController() {
  SetBtnsHandlers();
  SetP1High();
  CalibrateHandAccelerometer();
}