#include "buttons.h"
#include "orientation.h"
#include "pins.h"
#include "transmitter.h"
#include "view.h"
#include <MicroBit.h>

#ifndef HAND_CONTROLLER_H
#define HAND_CONTROLLER_H

#define MICROBIT_UBIT_AS_STATIC_OBJECT

#ifdef MICROBIT_UBIT_AS_STATIC_OBJECT
extern MicroBit uBit; // Target the same uBit as in main.cpp
#else
extern MicroBit &uBit;
#endif

#endif
