#include "EventModel.h"
#include "Timer.h"
#include "flightController.h"
#include <MicroBit.h>

int altitude = 0;

// Define measurement pins:
MicroBitPin &TRIG = uBit.io.P0;
MicroBitPin &ECHO = uBit.io.P1;

const uint32_t TRIG_HIGH_TIME_US = 10;
const int ECHO_TIMEOUT_US = 36000;

// EventModel
const int TRIG_EVENT_ID = 1001;
const int TRIG_EVENT_VALUE = 1;

static int calculateCmFromPulse() {
  // Track time where ECHO is high
  return ECHO.getPulseUs(ECHO_TIMEOUT_US) / 58;
}

static void onDisableTRIG(Event) {
  TRIG.setDigitalValue(0);
  altitude = calculateCmFromPulse();
}

static void initUltrasonicSensor() {
  // 60 ms measurement cycle
  // Using codal Event model to track the TRIG signal.
  EventModel::defaultEventBus->listen(TRIG_EVENT_ID, TRIG_EVENT_VALUE,
                                      onDisableTRIG);
}

static void startMeasurement() {
  // Apply TRIG pulse for TRIG_HIGH_TIME_US milliseconds.
  TRIG.setDigitalValue(1);
  uBit.timer.eventAfterUs(TRIG_HIGH_TIME_US, TRIG_EVENT_ID, TRIG_EVENT_VALUE);
}

void MeasureAltitudeLoop() {
  initUltrasonicSensor();

  while (true) { // measure one per second
    startMeasurement();
    uBit.display.scroll(altitude);
    uBit.sleep(1000);
  }
}