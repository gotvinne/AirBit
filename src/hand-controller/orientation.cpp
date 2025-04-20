#include <MicroBit.h>
#include "handController.h"

// orientation variables:
int pitch = 0;
int roll = 0;

const int PITCH_THRESHOLD = 45;
const int ROLL_THRESHOLD = 45;

static int checkTreshold(int value, int threshold) {
    if (value < -threshold) {
        return -threshold;
    } else if (value > threshold) {
        return threshold;
    } else {
        return value;
    }
}

void setOrientation() {
    int measuredPitch = uBit.accelerometer.getPitch();
    pitch = checkTreshold(measuredPitch, PITCH_THRESHOLD);

    int measuredRoll = uBit.accelerometer.getRoll();
    roll = checkTreshold(measuredRoll, ROLL_THRESHOLD);
    
    uBit.display.scroll(pitch);
    //uBit.display.scroll(roll);
}