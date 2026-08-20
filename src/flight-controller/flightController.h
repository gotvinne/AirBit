#pragma once
#include "BlackBoard/I2C.h"
#include "BlackBoard/imu.h"
#include "BlackBoard/servo.h"
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
  CCW1, // Upper right motor
  CW1,  // Upper left motor
  CW2,  // Lower right motor
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

const State &GetFlightState();

void SetState(State state);

void SetErrorMessage(const ManagedString &message);

ManagedString GetErrorMessage();

uint8_t *GetPropellerActuation();

void InitFlightController();

void UpdatePropellerActuationEqual(uint8_t actuation);

void MotorMixing(int throttle, int yaw, int pitch, int roll);

void CheckFlightState();

#endif