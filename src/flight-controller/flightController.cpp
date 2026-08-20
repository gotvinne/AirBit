#include "flightController.h"
#include "../utilities.h"

static FlightState flightState{
  altitude : 0,
  state : State::CALIBRATING,
  propellerActuation : {0, 0, 0, 0}, // ccw1_v, cw1_v, cw2_v, ccw2_v
  errorMessage : "",
};

static void updatePropellerActuation(MotorIndex motorIndex, uint8_t actuation) {
  flightState.propellerActuation[static_cast<uint8_t>(motorIndex)] = actuation;
}

void InitFlightController() {
  InitBatteryInfo();
  const State &currentState = GetFlightState();
  if (currentState == State::ERROR) {
    return;
  }
  InitI2C(); // NB! I2C must be initialized before the servo controller, as the
             // servo controller relies on I2C communication.
  if (currentState == State::ERROR) {
    return;
  }
  InitIMU();
  if (currentState == State::ERROR) {
    return;
  }
  InitServo();
  if (currentState == State::ERROR) {
    return;
  }
  CalibrateDroneAccelerometer();
}

const State &GetFlightState() { return flightState.state; }

uint8_t *GetPropellerActuation() { return flightState.propellerActuation; }

void SetState(State state) { flightState.state = state; }

void SetErrorMessage(const ManagedString &message) {
  flightState.errorMessage = message;
}

ManagedString GetErrorMessage() { return flightState.errorMessage; }

void UpdatePropellerActuationEqual(uint8_t actuation) {
  for (int i = 0; i < NUM_MOTORS; ++i) {
    updatePropellerActuation(static_cast<MotorIndex>(i), actuation);
  }
  SetPropellerActuation();
}

void MotorMixing(int throttle, int yaw, int pitch, int roll) {
  // Motor mixing algorithm for quadcopter in X configuration
  updatePropellerActuation(MotorIndex::CCW1,
                           throttle + yaw + pitch +
                               roll); // Front right motor (CCW)
  updatePropellerActuation(MotorIndex::CW1, throttle - yaw + pitch -
                                                roll); // Front left motor (CW)
  updatePropellerActuation(MotorIndex::CW2, throttle - yaw - pitch +
                                                roll); // Back right motor (CW)
  updatePropellerActuation(MotorIndex::CCW2, throttle + yaw - pitch -
                                                 roll); // Back left motor (CCW)
  SetPropellerActuation();
}

void CheckFlightState() {
  const State &currentState = GetFlightState();
  if (currentState == State::ARMED) {
    SetState(IsDroneArmed() ? State::ARMED : State::IDLE);
    return;
  }
  const BatteryState &batteryState = GetBatteryState();
  if (batteryState.isCharging &&
      !batteryState.debugging) { // If the battery is charging, set the
                                 // state to CHARGING again
    SetState(State::CHARGING);
    return;
  }
  if ((batteryState.batteryLevel == BatteryLevel::EMPTY ||
       batteryState.batteryLevel == BatteryLevel::LOW) &&
      !batteryState.debugging) {
    SetState(State::LOWBATTERY);
    return;
  } else {
    SetState(IsDroneArmed() ? State::ARMED : State::IDLE);
  }
}