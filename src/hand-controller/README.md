# Hand Controller Logic

### Buttons - Throttle Control (buttons.cpp)

Implementation: 
- There is an extern variable throttle which is shared among cpp modules (the purpose of extern)
- This variable is incremented / decremented by 5 based on the button callbacks. If the variable exeeds 40, it increments 1.
```
extern int throttle;
``` 

### Compass - Pitch / Roll Control (orientations.cpp)

Implementation: 
- The microbit has an IMU inbuilt which tracks the orientation of the device. 
- The codal lib provides methods on retrieving the pitch and roll using the accelerometer.

```
void Accelerometer::recalculatePitchRoll()
{
    double x = (double) sample.x;
    double y = (double) sample.y;
    double z = (double) sample.z;

    roll = atan2(x, -z);
    pitch = atan2(y, (x*sin(roll) - z*cos(roll)));
}
```

- The accelerometer bases its calculation on the SIMPLE CARTSESIAN coordinate space:

SIMPLE_CARTESIAN: Data will be returned based on an easy to understand alignment, consistent with the cartesian system taught in schools.

When held upright, facing the user:
```
                           /
    +--------------------+ z
    |                    |
    |       .....        |
    | *     .....      * |
 ^  |       .....        |
 |  |                    |
 y  +--------------------+  x-->
 ```

- A positive pitch is given when the controller is rotated counter-clock-wise around the x-axis
- A positive roll is given when the controller is rotated clock-wise around the y-axis
This is illustrated in the picture below:

![img/orientations.png](../../img/orientations.png)