#include "flightController.h"
#include <MicroBit.h>
#include <algorithm>
#include <cmath>

const double BATTERY_FACTOR = 5.94; // conversion from ADC reading to mV
const int UPPER_BATTERY_MV = 4200;
const int LOWER_BATTERY_MV = 3400;
const double ALPHA = 0.7; // Smoothing factor [0,1]
const int CHARGING_THRESHOLD = 750;

int batteryMilliVolt = 3800;
BatteryLevel batteryLevel = BatteryLevel::MEDIUMLOW;
bool isCharging = false;

static int readBatteryMilliVolt() {
  int p0 = uBit.io.P0.getAnalogValue();
  isCharging = p0 >= CHARGING_THRESHOLD;
  return p0 * BATTERY_FACTOR;
}

static double smoothMeasurement(int x) {
  // using Exponential moving average: (low pass filter)
  // y_n = alpha * x + (1-alpha) * y_(n-1)
  return std::round(ALPHA * x + ((1 - ALPHA) * batteryMilliVolt));
}

static BatteryLevel milliVoltToBatteryLevel(int batteryMilliVolt) {
  int level =
      std::max(LOWER_BATTERY_MV, std::min(UPPER_BATTERY_MV, batteryMilliVolt));
  level = std::round(21 - (batteryMilliVolt / 200));
  return static_cast<BatteryLevel>(level);
}

void SetBatteryInfo() {
  double meas = readBatteryMilliVolt();
  batteryMilliVolt = smoothMeasurement(meas);
  batteryLevel = milliVoltToBatteryLevel(batteryMilliVolt);
  // uBit.display.scroll(static_cast<uint16_t>(batteryLevel));
}

/*
 * Calibrating battery
 */
void InitBatteryInfo() {
  for (int i = 0; i < 5; i++) {
    double meas = readBatteryMilliVolt();
    batteryMilliVolt = smoothMeasurement(meas);
    batteryLevel = milliVoltToBatteryLevel(batteryMilliVolt);
    // uBit.display.scroll(static_cast<uint16_t>(batteryLevel));
    // uBit.sleep(100);
  }
}