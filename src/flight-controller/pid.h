#pragma once

struct PIDOutput {
  int pitch;
  int roll;
};

void CalibrateDroneAccelerometer();
void SetPIDActuation();