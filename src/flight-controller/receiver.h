#pragma once

// Reference variables:
bool IsDroneArmed();
const int &GetDroneThrottle();
const int &GetDroneRoll();
const int &GetDronePitch();
const int &GetDroneYaw();

void FlushRadioBuffer();
