#include <MicroBit.h>
#include "hand-controller/handController.h"

MicroBit uBit;

int main()
{
    uBit.init();

    setBtnsHandlers();

    while(1)
    {
        setOrientation();
        updateDisplay();
        uBit.sleep(100);
    }
    return 0;
}