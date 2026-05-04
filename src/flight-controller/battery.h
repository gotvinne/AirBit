#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

enum class BatteryLevel : uint16_t {
  FULL = 0,
  MEDIUMFULL = 1,
  MEDIUMLOW = 2,
  LOW = 3,
  EMPTY = 4
};

struct BatteryState {
  BatteryLevel batteryLevel;
  bool isCharging;
};

const BatteryState &GetBatteryState();

void InitBatteryInfo();

void SetBatteryInfo();