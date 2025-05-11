#include <MicroBit.h>
#include <cmath>
#include "handController.h"

// Microit display
const uint16_t LED_DISPLAY_SIZE = 5;
const int LED_ON = 255;

// Pitch / Roll threshold
const int FIRST_DEGREE_THRESHOLD = 11;
const int SECOND_DEGREE_THRESHOLD = 34;

static void displayArmed(Image& ledDisplay) {
    if (armed) {
        ledDisplay.setPixelValue(0, 0, LED_ON);
    } 
}

static void displayThrottle(Image& ledDisplay) {
    if (throttle == 0) {
        return;
    } else {
        int y = floor(4 - (throttle / 25));
        ledDisplay.setPixelValue(0, y, LED_ON);
    } 
}

static void displayPitchRoll(Image& ledDisplay) {
    uint16_t x, y;

    if (roll <= -SECOND_DEGREE_THRESHOLD) {
        x = 0;
    } else if (roll < -FIRST_DEGREE_THRESHOLD) {
        x = 1;
    } else if (roll <= FIRST_DEGREE_THRESHOLD) {
        x = 2;
    } else if (roll < SECOND_DEGREE_THRESHOLD) {
        x = 3;
    } else {
        x = 4;
    }

    if (pitch <= -SECOND_DEGREE_THRESHOLD) {
        y = 0;
    } else if (pitch < -FIRST_DEGREE_THRESHOLD) {
        y = 1;
    } else if (pitch <= FIRST_DEGREE_THRESHOLD) {
        y = 2;
    } else if (pitch < SECOND_DEGREE_THRESHOLD) {
        y = 3;
    } else {
        y = 4;
    }

    ledDisplay.setPixelValue(x, y, LED_ON);
}

void updateDisplay() {
    uBit.display.clear();

    Image ledDisplay = Image(LED_DISPLAY_SIZE, LED_DISPLAY_SIZE);
    displayArmed(ledDisplay);
    displayThrottle(ledDisplay);
    displayPitchRoll(ledDisplay);
    uBit.display.print(ledDisplay);
}
