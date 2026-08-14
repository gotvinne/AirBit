#include "I2C.h"
#include "../flightController.h"

MicroBitPin SDA = uBit.io.P2;
MicroBitPin SCL = uBit.io.P1;

void InitI2C() {
  if (uBit.i2c.redirect(uBit.io.P2, uBit.io.P1)) {
    SetState(State::ERROR);
    SetErrorMessage("I2C");
  }
}