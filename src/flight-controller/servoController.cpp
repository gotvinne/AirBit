#include "flightController.h"
#include <MicroBit.h>

// I2C
const uint16_t SERVO_CONTROLLER_ADDR = 196; // 8-bit SLAVE ADDRESS
MicroBitPin SDA = uBit.io.P2;
MicroBitPin SCL = uBit.io.P1;

// Servo Motors
const uint8_t MOTOR_CW1_REG = 4;
const uint8_t MOTOR_CCW1_REG = 2;
const uint8_t MOTOR_CW2_REG = 3;
const uint8_t MOTOR_CCW2_REG = 5;

static void SetPropellerActuation(uint8_t motor, uint8_t actuation) {
  uint8_t buf[2] = {motor, actuation};
  uBit.i2c.write(SERVO_CONTROLLER_ADDR, buf, 2);
}

void SetAllPropellerActuation(uint8_t cw1_v, uint8_t cw2_v, uint8_t ccw1_v,
                              uint8_t ccw2_v) {
  SetPropellerActuation(MOTOR_CW1_REG, cw1_v);
  SetPropellerActuation(MOTOR_CW2_REG, cw2_v);
  SetPropellerActuation(MOTOR_CCW1_REG, ccw1_v);
  SetPropellerActuation(MOTOR_CCW2_REG, ccw2_v);
}

void InitServoController() {
  uBit.i2c.redirect(uBit.io.P2, uBit.io.P1);

  uint8_t d1[2] = {0, 128};
  uBit.i2c.write(SERVO_CONTROLLER_ADDR, d1, 2);
  uint8_t d2[2] = {1, 5};
  uBit.i2c.write(SERVO_CONTROLLER_ADDR, d2, 2);
  uint8_t d3[2] = {8, 170};
  uBit.i2c.write(SERVO_CONTROLLER_ADDR, d3, 2);
}