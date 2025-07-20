#include <MicroBit.h>

#ifndef HAND_CONTROLLER_H
#define HAND_CONTROLLER_H

#define MICROBIT_UBIT_AS_STATIC_OBJECT

#ifdef MICROBIT_UBIT_AS_STATIC_OBJECT
extern MicroBit uBit; // Target the same uBit as in main.cpp

// Hand controller variables, PARTY
extern int pitch;
extern bool armed;
extern int roll;
extern int throttle;
extern int yaw;
#else
extern MicroBit &uBit;
#endif

// Setup peripherals:
void setBtnsHandlers();
void initRadio();
void setP1High();

// Monitoring:
void setOrientation();
void updateDisplay();
void checkPanic();
void transmittData();
void readPins();

#endif
