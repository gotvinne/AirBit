#include "flightController.h"

static FlightState flightState{
  altitude : 0,
  state : State::CALIBRATING,
  motorActuation : {0, 0, 0, 0},
};

void InitFlightController() {
  SetState(State::CALIBRATING);
  InitBatteryInfo();
  InitI2C(); // NB! I2C must be initialized before the servo controller, as the
             // servo controller relies on I2C communication.
  InitServoController();
}

const FlightState &GetFlightState() { return flightState; }

void SetState(State state) { flightState.state = state; }