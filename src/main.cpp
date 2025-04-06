#include <MicroBit.h>

MicroBit uBit;

int main()
{
    uBit.init();

    //Display 'Hello world!', forever!
    while (1)
        uBit.display.scroll("GEIR!");
}