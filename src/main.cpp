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
        uBit.sleep(1000);
    }
    return 0;
}