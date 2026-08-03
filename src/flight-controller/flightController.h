#pragma once
#include "WonderKit/I2C.h"
#include "WonderKit/servoController.h"
#include "battery.h"
#include "pid.h"
#include "receiver.h"
#include "ultrasonicSensor.h"
#include "view.h"
#include <MicroBit.h>
#include <cstdint>

// FlightController.h is the umbrella header for the flight controller module,
// which includes all the necessary headers for the flight controller
// implementation and exposes the necessary functions and variables to be used
// in main.cpp and other modules.

#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#define MICROBIT_UBIT_AS_STATIC_OBJECT

const int NUM_MOTORS = 4;

enum class State : uint8_t {
  CALIBRATING,
  ERROR,
  CHARGING,
  LOWBATTERY,
  IDLE,
  ARMED,
};

enum class MotorIndex : uint8_t {
  CW1,  // Upper left motor
  CW2,  // Lower right motor
  CCW1, // Upper right motor
  CCW2  // Lower left motor
};

struct FlightState {
  int altitude;
  State state;
  uint8_t propellerActuation[NUM_MOTORS];
  ManagedString errorMessage;
};

#ifdef MICROBIT_UBIT_AS_STATIC_OBJECT
extern MicroBit uBit; // Target the same uBit as in main.cpp

#else
extern MicroBit uBit;
#endif

const FlightState &GetFlightState();

void SetState(State state);

void SetErrorMessage(const ManagedString &message);

void InitFlightController();

void UpdatePropellerActuation(MotorIndex motorIndex, uint8_t actuation);

void UpdatePropellerActuationEqual(uint8_t actuation);

void CheckFlightState();

#endif