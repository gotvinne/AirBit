#include <MicroBit.h>
#include "handController.h"

// Button variables:
int throttle = 30;
const int THROTTLE_THRESHOLD = 40;

static void onButtonA(MicroBitEvent)
{
    if (throttle >= THROTTLE_THRESHOLD) {
        throttle += 1;
    } else {
        throttle += 5;
    }
    uBit.display.print(throttle);
}

static void onButtonB(MicroBitEvent)
{
    throttle -= 5;
    uBit.display.print(throttle);
}

void setBtnsHandlers() {
    uBit.messageBus.listen(DEVICE_ID_BUTTON_A, DEVICE_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(DEVICE_ID_BUTTON_B, DEVICE_BUTTON_EVT_CLICK, onButtonB);
}
