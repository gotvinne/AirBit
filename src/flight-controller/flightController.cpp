#include "flightController.h"
#include "../utilities.h"

static FlightState flightState{
  altitude : 0,
  state : State::CALIBRATING,
  propellerActuation : {0, 0, 0, 0}, // cw1_v, cw2_v, ccw1_v, ccw2_v
};

void InitFlightController() {
  InitBatteryInfo();
  InitI2C(); // NB! I2C must be initialized before the servo controller, as the
             // servo controller relies on I2C communication.
  InitServoController();
}

const FlightState &GetFlightState() { return flightState; }

void SetState(State state) { flightState.state = state; }

void UpdatePropellerActuation(MotorIndex motorIndex, uint8_t actuation) {
  flightState.propellerActuation[static_cast<uint8_t>(motorIndex)] = actuation;
}

void UpdatePropellerActuationEqual(uint8_t actuation) {
  for (int i = 0; i < NUM_MOTORS; ++i) {
    UpdatePropellerActuation(static_cast<MotorIndex>(i), actuation);
  }
  SetPropellerActuation();
}

void CheckFlightState() {
  if (GetFlightState().state == State::ARMED) {
    SetState(IsDroneArmed() ? State::ARMED : State::IDLE);
    return;
  }
  if (GetBatteryState().isCharging) { // If the battery is charging, set the
                                      // state to CHARGING again
    SetState(State::CHARGING);
    return;
  }
  if (GetBatteryState().batteryLevel == BatteryLevel::EMPTY ||
      GetBatteryState().batteryLevel == BatteryLevel::LOW) {
    SetState(State::LOWBATTERY);
    return;
  } else {
    SetState(IsDroneArmed() ? State::ARMED : State::IDLE);
  }
}