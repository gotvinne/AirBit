#pragma once

#include <MicroBit.h>
#include <cstdint>

struct GyroAttitude {
  int pitch;
  int roll;
  int yaw;
};

struct GyroState {
  bool isAvailable;
  GyroAttitude offsets;
  GyroAttitude ratesDps;
};

void InitGyro();
void UpdateGyroMeasurements();
int GetRollRate();
int GetPitchRate();
int GetYawRate();
bool IsGyroAvailable();
