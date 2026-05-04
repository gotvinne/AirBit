#include "servoController.h"
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

static void FailSafe() {
  SetState(State::PANIC);
  uBit.display.scroll("I2C");
}

static void WriteToServoController(uint8_t reg, uint8_t value) {
  uint8_t buf[2] = {reg, value};
  if (uBit.i2c.write(SERVO_CONTROLLER_ADDR, buf, 2)) {
    FailSafe();
  }
}

static void ReadServoController(uint8_t reg, uint8_t value) {
  uint8_t buf[2] = {reg, value};
  if (uBit.i2c.write(SERVO_CONTROLLER_ADDR, buf, 2)) {
    FailSafe();
  }
}

static void SetPropellerActuation(uint8_t motor, uint8_t actuation) {
  uint8_t buf[2] = {motor, actuation};
  if (uBit.i2c.write(SERVO_CONTROLLER_ADDR, buf, 2)) {
    SetState(State::PANIC);
    uBit.display.scroll("Motor Error");
  }
}

void SetAllPropellerActuation(uint8_t cw1_v, uint8_t cw2_v, uint8_t ccw1_v,
                              uint8_t ccw2_v) {
  SetPropellerActuation(MOTOR_CW1_REG, cw1_v);
  SetPropellerActuation(MOTOR_CW2_REG, cw2_v);
  SetPropellerActuation(MOTOR_CCW1_REG, ccw1_v);
  SetPropellerActuation(MOTOR_CCW2_REG, ccw2_v);
}

void InitServoController() {
  // Redirect I2C to use P1 and P2.
  if (uBit.i2c.redirect(uBit.io.P2, uBit.io.P1)) {
    FailSafe();
  }

  // Configure the servo controller with default values.
  WriteToServoController(0, 128);
  WriteToServoController(1, 5);
  WriteToServoController(8, 170);
  SetAllPropellerActuation(0, 0, 0, 0);
  uint8_t buf[2] = {0, 0};
  if (uBit.i2c.read(SERVO_CONTROLLER_ADDR, buf, 2)) {
    FailSafe();
  } else {
    uBit.display.print("M");
    uBit.sleep(3000);
  }
}