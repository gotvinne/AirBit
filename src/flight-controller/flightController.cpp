#include "flightController.h"

static FlightState flightState{
  altitude : 0,
  state : State::CALIBRATING,
  motorActuation : {0, 0, 0, 0},
};

const FlightState &GetFlightState() { return flightState; }

void SetState(State state) { flightState.state = state; }