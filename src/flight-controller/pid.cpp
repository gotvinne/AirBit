#include "pid.h"
#include "flightController.h"

const uint8_t ARMED_THROTTLE = 5;

void SetThrottle() {
  uint8_t throttle = GetDroneThrottle() * 2.55;
  if (throttle == 0) {
    UpdatePropellerActuationEqual(ARMED_THROTTLE);
  } else {
    UpdatePropellerActuationEqual(throttle);
  }
}
