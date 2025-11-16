#include <MicroBit.h>

#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#define MICROBIT_UBIT_AS_STATIC_OBJECT

#ifdef MICROBIT_UBIT_AS_STATIC_OBJECT
extern MicroBit uBit; // Target the same uBit as in main.cpp

extern int altitude;
#else
extern MicroBit &uBit;
#endif

// Setup peripherals:
void initUltrasonicSensor();

// Monitoring:
void startMeasurement();

#endif