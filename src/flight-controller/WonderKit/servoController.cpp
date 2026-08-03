#include "servoController.h"
#include "../flightController.h"
#include "../utilities.h"
#include "I2C.h"
#include <MicroBit.h>

const uint16_t SERVO_CONTROLLER_ADDR = 196; // 8-bit SLAVE ADDRESS

// Servo controller registers
const uint8_t SERVO_CONTROLLER_MODE1_REG = 0;
const uint8_t SERVO_CONTROLLER_MODE2_REG = 1;
const uint8_t SERVO_CONTROLLER_MODE3_REG = 8;
// Servo controller register values
// https://github.com/gomakekit/Airbit_V2/blob/master/custom.ts#L459
const uint8_t SERVO_CONTROLLER_MODE1_VALUE = 128;
const uint8_t SERVO_CONTROLLER_MODE2_VALUE = 5;
const uint8_t SERVO_CONTROLLER_MODE3_VALUE = 170;

// Servo Motors
const uint8_t MOTOR_CW1_REG = 4;
const uint8_t MOTOR_CCW1_REG = 2;
const uint8_t MOTOR_CW2_REG = 3;
const uint8_t MOTOR_CCW2_REG = 5;
const uint8_t motorRegs[] = {MOTOR_CW1_REG, MOTOR_CW2_REG, MOTOR_CCW1_REG,
                             MOTOR_CCW2_REG};

const int MAX_ACTUATION = 255;
const int MIN_ACTUATION = 0;

static uint8_t lastPropellerActuation[NUM_MOTORS] = {0, 0, 0, 0};

static void failSafe() {
  SetState(State::ERROR);
  SetErrorMessage("NM"); // No Motor detected
}

static void WriteToServoController(uint8_t reg, uint8_t value) {
  if (uBit.i2c.writeRegister(SERVO_CONTROLLER_ADDR, reg, value)) {
    failSafe();
  }
}

static void readServoController() {
  uint8_t buf[2] = {0, 0};
  if (uBit.i2c.read(SERVO_CONTROLLER_ADDR, buf, 2)) {
    failSafe();
  } else {
    uBit.display.print("M"); // Motor detected
    uBit.sleep(3000);
  }
}

static void WritePropellerActuation(uint8_t motor, uint8_t actuation) {
  actuation = Clamp(actuation, MIN_ACTUATION, MAX_ACTUATION);
  if (uBit.i2c.writeRegister(SERVO_CONTROLLER_ADDR, motor, actuation)) {
    failSafe();
    return;
  }

  uBit.sleep(10);
}

void SetPropellerActuation() {
  const uint8_t *motorValues = GetFlightState().propellerActuation;
  for (int i = 0; i < NUM_MOTORS; ++i) {
    if (motorValues[i] == lastPropellerActuation[i]) {
      continue; // Skip writing to the servo controller if the actuation value
                // hasn't changed
    }
    WritePropellerActuation(motorRegs[i], motorValues[i]);
    lastPropellerActuation[i] = motorValues[i];
  }
}

void InitServoController() {
  // Configure the servo controller with default values.
  WriteToServoController(SERVO_CONTROLLER_MODE1_REG,
                         SERVO_CONTROLLER_MODE1_VALUE);
  WriteToServoController(SERVO_CONTROLLER_MODE2_REG,
                         SERVO_CONTROLLER_MODE2_VALUE);
  WriteToServoController(SERVO_CONTROLLER_MODE3_REG,
                         SERVO_CONTROLLER_MODE3_VALUE);
  SetPropellerActuation(); // Set all motors to 0 actuation due to initial
                           // state.
  // Detect WonderKit powered.
  readServoController();
}