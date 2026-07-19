#pragma once
#include <cstdint>

void InitServoController();

void SetAllPropellerActuation(uint8_t cw1_v, uint8_t cw2_v, uint8_t ccw1_v,
                              uint8_t ccw2_v);