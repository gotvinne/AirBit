#pragma once

#include <MicroBit.h>
#include <cstdint>

struct IMUOffsets {
  int accX;
  int accY;

  int gyroX;
  int gyroY;
  int gyroZ;
};

struct IMUMeasurements {
  int pitch;
  int roll;

  int pitchRate;
  int rollRate;
  int yawRate;
};

struct IMUState {
  bool isAvailable;
  IMUMeasurements imu;
  IMUOffsets offsets;
};

void InitIMU();
void UpdateIMUMeasurements();
int GetRollRate();
int GetPitchRate();
int GetYawRate();
bool IsIMUAvailable();
