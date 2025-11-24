#include "flightController.h"
#include <MicroBit.h>
#include <cmath>

const double BATTERY_FACTOR = 5.94;
const double alpha = 0.2; // Smoothing factor
int batteryMilliVolt = 3800;


static int readBatteryMilliVolt() {
    int p0 = uBit.io.P0.getAnalogValue();
    return p0 * BATTERY_FACTOR;
}

static double smoothMeasurement(int x) {
    // using Exponential moving average: (low pass filter)
    // y_n = alpha * x + (1-alpha) * y_(n-1)
    return std::round(alpha * x + ((1 - alpha) * batteryMilliVolt));
}

void BatteryInfo() {
    double meas = readBatteryMilliVolt();
    batteryMilliVolt = smoothMeasurement(meas);
    uBit.display.scroll(batteryMilliVolt);
}