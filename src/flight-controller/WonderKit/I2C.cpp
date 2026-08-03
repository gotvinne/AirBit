#include "I2C.h"
#include "../flightController.h"

MicroBitPin SDA = uBit.io.P2;
MicroBitPin SCL = uBit.io.P1;

void InitI2C() {
  // Redirect I2C to use P1 and P2.
  if (uBit.i2c.redirect(uBit.io.P2, uBit.io.P1)) {
    SetState(State::ERROR);
    uBit.display.print("I2C");
  }
}