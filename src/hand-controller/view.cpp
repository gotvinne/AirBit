#include "view.h"
#include "../utilities.h"
#include "buttons.h"
#include "handController.h"
#include "orientation.h"
#include "pins.h"
#include <MicroBit.h>

// MicroBit display
const uint16_t LED_DISPLAY_SIZE = 5;

void UpdateDisplay() {
  uBit.display.clear();

  Image ledDisplay = Image(LED_DISPLAY_SIZE, LED_DISPLAY_SIZE);
  DisplayArmed(IsArmed(), ledDisplay);
  DisplayThrottle(GetThrottle(), ledDisplay);
  DisplayPitchRoll(GetRoll(), GetPitch(), ledDisplay);
  DisplayYaw(GetYaw(), ledDisplay);
  uBit.display.print(ledDisplay);
}
