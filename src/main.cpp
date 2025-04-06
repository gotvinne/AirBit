#include <MicroBit.h>
#include "hand-controller/handController.h"

MicroBit uBit;

int main()
{
    uBit.init();

    setBtnsHandlers();

    uBit.display.print(throttle);
    while(1)
        uBit.sleep(1000);

    return 0;
}

