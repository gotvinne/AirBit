#pragma once

#include <MicroBit.h>
#include <cstdint>

struct GyroState {
  bool isAvailable;
  int rollOffset;
  int pitchOffset;
  int yawOffset;
  int rollRate;
  int pitchRate;
  int yawRate;
};

void InitGyro();
void UpdateGyroMeasurements();
const int &GetRollRate();
const int &GetPitchRate();
const int &GetYawRate();
bool IsGyroAvailable();
