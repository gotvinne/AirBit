#include <MicroBit.h>

#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#define MICROBIT_UBIT_AS_STATIC_OBJECT

enum class BatteryLevel : uint16_t {
  FULL = 0,
  MEDIUMFULL = 1,
  MEDIUMLOW = 2,
  LOW = 3,
  EMPTY = 4
};

#ifdef MICROBIT_UBIT_AS_STATIC_OBJECT
extern MicroBit uBit; // Target the same uBit as in main.cpp

extern int altitude;

extern BatteryLevel batteryLevel;
extern bool isCharging;
#else
extern MicroBit &uBit;
#endif

void MeasureAltitudeLoop();

void InitBatteryInfo();

void SetBatteryInfo();

void UpdateDisplay();

#endif