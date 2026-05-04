#pragma once
#include "battery.h"
#include "servoController.h"
#include "ultrasonicSensor.h"
#include "view.h"
#include <MicroBit.h>

// FlightController.h is the umbrella header for the flight controller module,
// which includes all the necessary headers for the flight controller
// implementation and exposes the necessary functions and variables to be used
// in main.cpp and other modules.

#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#define MICROBIT_UBIT_AS_STATIC_OBJECT

enum class State : uint16_t { CALIBRATING, CHARGING, DISARMED, ARMED, PANIC };

struct FlightState {
  int altitude;
  State state;
  uint8_t motorActuation[4];
};

#ifdef MICROBIT_UBIT_AS_STATIC_OBJECT
extern MicroBit uBit; // Target the same uBit as in main.cpp

#else
extern MicroBit uBit;
#endif

const FlightState &GetFlightState();

void SetState(State state);

#endif