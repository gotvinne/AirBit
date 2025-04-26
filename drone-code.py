# Draw battery icon
# Level 4: full
# Level 3: medium-high
# Level 2: medium-low
# Level 1: low
# Level 0: Empty
def batteryIcon(level: number):
    led.plot(2, 4)
    led.plot(2, 0)
    for index in range(4):
        led.plot(1, index + 1)
        led.plot(3, index + 1)
    if level > 0:
        index2 = 0
        while index2 <= level:
            led.plot(2, 5 - index2)
            index2 += 1
    else:
        led.plot(0, 0)
        led.plot(2, 2)
        led.plot(4, 4)
def servo1_test():
    pins.digital_write_pin(DigitalPin.P1, 1)
    control.wait_micros(1500 + roll * 10)
    pins.digital_write_pin(DigitalPin.P1, 0)
def JoystickDeadBand():
    global roll, pitch
    if abs(roll) < 5:
        roll = 0
    if abs(pitch) < 5:
        pitch = 0
def screen():
    # If charging is detected
    if pins.analog_read_pin(AnalogReadWritePin.P0) > 750:
        # Charge mode can not happen when drone is flying
        if arm == 0:
            charging()
    else:
        if mode == 0:
            dots()
        if mode == 1:
            led.plot_bar_graph(airbit.battery_level(), 100)
        if mode == 2:
            basic.show_number(throttle)
        if mode == 3:
            motorTest()
        if mode == 4:
            basic.clear_screen()
            motorLed()
        if mode == 5:
            basic.show_number(airbit.batterym_volt())
def mainLoop():
    global stable, cpuTime, startTime
    while True:
        # Read raw data from gyro and accelerometer
        airbit.IMU_sensorRead()
        # Find drone's absolute Roll, Pitch and Yaw angles with sensor fusion, gyro and accelerometer together.
        airbit.calculate_angles()
        basic.pause(1)
        lostSignalCheck()
        if motorTesting == False:
            # The "magic" algorithm that stabilises the drone based on setpoint angle and actual angle, finding the difference and chanring motor speed to compensate.
            airbit.stabilise_pid()
        # If upside down while armed, disable flying
        if abs(imuRoll) > 90:
            stable = False
        else:
            stable = True
        # Only start motors if armed, stable, motor controller and gyro is operating
        if arm and stable and (mcExists and (gyroExists and notCharging)):
            if throttle == 0:
                # Idle speed of motors
                airbit.motor_speed(5, 5, 5, 5)
            else:
                airbit.motor_speed(motorA, motorB, motorC, motorD)
        else:
            # Clear registers for error compensation algorithms, do not keep errors from past flight.
            airbit.clean_reg()
            if motorTesting:
                airbit.motor_speed(motorA, motorB, motorC, motorD)
            else:
                airbit.motor_speed(0, 0, 0, 0)
        cpuTime = input.running_time() - startTime
        startTime = input.running_time()

def on_button_pressed_a():
    global mode
    mode += -1
    if mode < 0:
        mode = 5
input.on_button_pressed(Button.A, on_button_pressed_a)

def radioSendData():
    radio.send_value("Pd", Math.round(imuPitch))
    radio.send_value("Rd", Math.round(imuRoll))
    radio.send_value("Td", input.temperature())
    radio.send_value("B", batterymVoltSmooth)
    radio.send_value("G", input.acceleration(Dimension.Z))
    radio.send_value("S", input.sound_level())
def gyroAccBubble():
    pass

def on_button_pressed_ab():
    global mode
    mode = 0
input.on_button_pressed(Button.AB, on_button_pressed_ab)

def on_button_pressed_b():
    global mode
    mode += 1
    if mode > 5:
        mode = 0
input.on_button_pressed(Button.B, on_button_pressed_b)

def motorLed():
    basic.clear_screen()
    led.plot_brightness(0, 4, motorA)
    led.plot_brightness(0, 0, motorB)
    led.plot_brightness(4, 4, motorC)
    led.plot_brightness(4, 0, motorD)
    led.plot(Math.map(imuRoll, -15, 15, 0, 4),
        Math.map(imuPitch, -15, 15, 4, 0))

def on_received_value(name, value):
    global radioReceivedTime, pitch, arm, roll, throttle, yaw
    radioReceivedTime = input.running_time()
    if name == "P":
        pitch = expo(value) / -3
        pitch = Math.constrain(pitch, -15, 15)
    if name == "A":
        arm = value
    if name == "R":
        roll = expo(value) / 3
        roll = Math.constrain(roll, -15, 15)
    if name == "T":
        throttle = value
        throttle = Math.constrain(throttle, 0, 100)
        if batterymVoltSmooth < 3400:
            throttle = Math.constrain(throttle, 0, 75)
    if name == "Y":
        yaw += value * 0.1
radio.on_received_value(on_received_value)

# smartBar(0, throttle)
# smartBar(4, airbit.batteryLevel())
def dots():
    basic.clear_screen()
    led.plot(Math.map(roll, -15, 15, 0, 4),
        Math.map(pitch, -15, 15, 4, 0))
    led.plot(Math.map(yaw, -30, 30, 0, 4), 4)
    if arm:
        led.plot(0, 0)
    if stable and (mcExists and (gyroExists and notCharging)):
        led.plot(2, 0)
    airbit.smart_bar(0, throttle)
    airbit.smart_bar(4, airbit.battery_level())
def charging():
    global AnalogP0Switch, notCharging
    if AnalogP0Switch - pins.analog_read_pin(AnalogReadWritePin.P0) > 20:
        basic.show_string("Connect battery", 100)
    else:
        basic.pause(200)
    AnalogP0Switch = pins.analog_read_pin(AnalogReadWritePin.P0)
    if pins.analog_read_pin(AnalogReadWritePin.P0) < 900:
        basic.clear_screen()
        for index3 in range(4):
            batteryIcon(index3 + 1)
            basic.pause(500)
        notCharging = False
    else:
        basic.show_icon(IconNames.YES)
        basic.show_string("Charge finished!")
        notCharging = True
        AnalogP0Switch += 1
def lostSignalCheck():
    global roll, pitch, yaw, throttle, arm
    # Failsafe makes only sense if already flying
    if throttle > 65 and arm:
        if input.running_time() > radioReceivedTime + 3000:
            roll = 0
            pitch = 0
            yaw = 0
            throttle = 65
        if input.running_time() > radioReceivedTime + 8000:
            roll = 0
            pitch = 0
            yaw = 0
            throttle = 0
            arm = 0
def motorTest():
    global motorA, motorB, motorC, motorD, motorTesting
    motorA = 0
    motorB = 0
    motorC = 0
    motorD = 0
    motorTesting = True
    motorB = 5
    for index4 in range(50):
        basic.clear_screen()
        airbit.rotate_dot(1, 1, 1, 10)
        basic.pause(20)
    motorB = 0
    motorD = 5
    for index5 in range(50):
        basic.clear_screen()
        airbit.rotate_dot(3, 1, 1, -10)
        basic.pause(20)
    motorD = 0
    motorC = 5
    for index6 in range(50):
        basic.clear_screen()
        airbit.rotate_dot(3, 3, 1, 10)
        basic.pause(20)
    motorC = 0
    motorA = 5
    for index7 in range(50):
        basic.clear_screen()
        airbit.rotate_dot(1, 3, 1, -10)
        basic.pause(20)
    motorA = 0
    motorTesting = False
def simpleBatteryCheck():
    basic.clear_screen()
    if airbit.batterym_volt() > 4050:
        batteryIcon(4)
    elif airbit.batterym_volt() > 3900:
        batteryIcon(3)
    elif airbit.batterym_volt() > 3750:
        batteryIcon(2)
    elif airbit.batterym_volt() > 3600:
        batteryIcon(1)
    else:
        batteryIcon(0)
def expo(inp: number):
    if inp >= 0:
        return inp / expoSetting + inp * inp / expoFactor
    else:
        return inp / expoSetting - inp * inp / expoFactor
AnalogP0Switch = 0
yaw = 0
radioReceivedTime = 0
startTime = 0
cpuTime = 0
motorTesting = False
throttle = 0
mode = 0
pitch = 0
roll = 0
arm = 0
expoFactor = 0
expoSetting = 0
motorD = 0
motorB = 0
motorC = 0
motorA = 0
batterymVoltSmooth = 0
imuRoll = 0
imuPitch = 0
notCharging = False
stable = False
gyroExists = False
mcExists = False
imuYaw = 0
batteryVolt = 0
mcExists = False
gyroExists = False
stable = True
notCharging = True
radioGroup = 7
basic.show_number(radioGroup)
simpleBatteryCheck()
basic.pause(1000)
imuPitch = 0
imuRoll = 0
batterymVoltSmooth = 3700
# Default: 0.7
rollPitchP = 0.9
rollPitchI = 0.004
# Default: 15
rollPitchD = 15
# Default: 4
yawP = 5
# Default: 10
yawD = 70
motorA = 0
motorC = 0
motorB = 0
motorD = 0
expoSetting = 2
expoFactor = 45 * 45 / (45 - 45 / expoSetting)
radio.set_group(radioGroup)
i2crr.set_i2c_pins(DigitalPin.P2, DigitalPin.P1)
# i2crr.setI2CPins(DigitalPin.P2, DigitalPin.P1)
basic.pause(100)
airbit.IMU_Start()
basic.pause(100)
airbit.PCA_Start()
while mcExists == False:
    airbit.PCA_Start()
    basic.show_string("Connect battery", 100)
basic.show_string("M")
airbit.IMU_gyro_calibrate()
while arm:
    basic.show_string("Disarm!")

def on_forever():
    mainLoop()
basic.forever(on_forever)

def on_forever2():
    airbit.battery_calculation()
basic.forever(on_forever2)

def on_forever3():
    if stable == False and arm:
        basic.show_string("Tilted. Please reset.")
    elif batterymVoltSmooth > 3400:
        screen()
    elif batterymVoltSmooth > 3350:
        batteryIcon(1)
    else:
        basic.clear_screen()
        batteryIcon(0)
        basic.pause(1000)
        basic.clear_screen()
        basic.pause(1000)
basic.forever(on_forever3)
