#include "flightController.h"
#include "../utilities.h"

static FlightState flightState{
  altitude : 0,
  state : State::CALIBRATING,
  propellerActuation : {0, 0, 0, 0}, // cw1_v, cw2_v, ccw1_v, ccw2_v
  errorMessage : "",
};

void InitFlightController() {
  InitBatteryInfo();
  if (GetFlightState().state == State::ERROR) {
    return;
  }
  InitI2C(); // NB! I2C must be initialized before the servo controller, as the
             // servo controller relies on I2C communication.
  if (GetFlightState().state == State::ERROR) {
    return;
  }
  InitGyro();
  if (GetFlightState().state == State::ERROR) {
    return;
  }
  InitServo();
  if (GetFlightState().state == State::ERROR) {
    return;
  }
  CalibrateAccelerometer();
}

const FlightState &GetFlightState() { return flightState; }

void SetState(State state) { flightState.state = state; }

void SetErrorMessage(const ManagedString &message) {
  flightState.errorMessage = message;
}

void UpdatePropellerActuation(MotorIndex motorIndex, uint8_t actuation) {
  flightState.propellerActuation[static_cast<uint8_t>(motorIndex)] = actuation;
}

void UpdatePropellerActuationEqual(uint8_t actuation) {
  for (int i = 0; i < NUM_MOTORS; ++i) {
    UpdatePropellerActuation(static_cast<MotorIndex>(i), actuation);
  }
  SetPropellerActuation();
}

void MotorMixingAlg(int throttle, int yaw, int pitch, int roll) {
  // Motor mixing algorithm for quadcopter in X configuration
  UpdatePropellerActuation(MotorIndex::CCW1,
                           throttle + yaw + pitch +
                               roll); // Front right motor (CCW)
  UpdatePropellerActuation(MotorIndex::CW1, throttle - yaw + pitch -
                                                roll); // Front left motor (CW)
  UpdatePropellerActuation(MotorIndex::CW2, throttle - yaw - pitch +
                                                roll); // Back right motor (CW)
  UpdatePropellerActuation(MotorIndex::CCW2, throttle + yaw - pitch -
                                                 roll); // Back left motor (CCW)
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