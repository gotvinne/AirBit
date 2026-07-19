#include "utilities.h"

static int pulseDirection = 15;
static int armedPulse = 0;

static const int LED_ON = 255;

static uint16_t angleToDisplayCoord(int angle) {
  return static_cast<uint16_t>(std::round((angle + 45) / 22.5));
}

static void updatePulse() {
  armedPulse += pulseDirection;

  if (armedPulse == 255) {
    pulseDirection = -15;
  } else if (armedPulse == 0) {
    pulseDirection = 15;
  }
}

void DisplayArmed(bool isArmed, Image &ledDisplay) {
  if (isArmed) {
    updatePulse();
    ledDisplay.setPixelValue(0, 0, armedPulse);
  } else {
    armedPulse = 0;
  }
}

void DisplayThrottle(int throttle, Image &ledDisplay) {
  if (throttle == 0) {
    return;
  } else {
    int y = floor(4 - (throttle / 25));
    ledDisplay.setPixelValue(0, y, LED_ON);
  }
}

void DisplayPitchRoll(int roll, int pitch, Image &ledDisplay) {
  uint16_t x = angleToDisplayCoord(roll);
  uint16_t y = angleToDisplayCoord(pitch);
  ledDisplay.setPixelValue(x, y, LED_ON);
}

void DisplayYaw(int yaw, Image &ledDisplay) {
  uint16_t x = static_cast<uint16_t>(yaw / 30 + 2);
  ledDisplay.setPixelValue(x, 0, LED_ON);
}

int Clamp(int value, int min, int max) {
  return (value < min) ? min : (value > max) ? max : value;
}