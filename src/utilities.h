#pragma once
#include <MicroBit.h>

void DisplayArmed(bool isArmed, Image &ledDisplay);

void DisplayThrottle(int throttle, Image &ledDisplay);

void DisplayPitchRoll(int roll, int pitch, Image &ledDisplay);

void DisplayYaw(int yaw, Image &ledDisplay);

int Clamp(int value, int min, int max);