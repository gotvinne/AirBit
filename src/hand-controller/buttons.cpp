#include <MicroBit.h>
#include "handController.h"

// Button variables:
int throttle = 0;
bool armed = false;

const int THROTTLE_THRESHOLD = 40;
const int THROTTLE_MIN = 0;

static void onButtonA(MicroBitEvent)
{
    if (throttle >= THROTTLE_THRESHOLD) {
        throttle += 1;
    } else {
        throttle += 5;
    }
    //uBit.display.print(throttle);
}

static void onButtonB(MicroBitEvent)
{
    if (throttle <= THROTTLE_MIN) {
        return;
    }
    throttle -= 5;
    //uBit.display.print(throttle);
}

static void onButtonAB(MicroBitEvent)
{
    armed = !armed;
    //uBit.display.print(armed ? "1" : "0");
}

void setBtnsHandlers() {
    uBit.messageBus.listen(DEVICE_ID_BUTTON_A, DEVICE_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(DEVICE_ID_BUTTON_B, DEVICE_BUTTON_EVT_CLICK, onButtonB);
    uBit.messageBus.listen(DEVICE_ID_BUTTON_AB, DEVICE_BUTTON_EVT_CLICK, onButtonAB);
}
