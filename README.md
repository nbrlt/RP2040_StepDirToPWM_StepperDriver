# RP2040 Unipolar StepDir Driver
This is a simple arduino code to use the PIOs of the RP2040 as logic to convert a Step/Dir interface into 4 signals used for PWM type bridge drivers such as the DRV8803 or DRV8932. 

The logic is coded with 1/2 steps commonly used with unipolar steppers, but it could be easily transformed into something else (even into bipolar logic) by modifying the pulse sequence in RP2040_StepDir_motorlogic.h.

## Usage
Copy the the library into Arduino/libraries and compile the arduino sketch. The RP2040 and this library can control up to 4 motors. When you define the motor port, 5 consecutive pins will be allocated to it, the last one being the step input, the rest being the control signal for the bridge driver. The dir pin is used through interrupts to control the direction of the motor.

The extra pins defined in the sketch (enable, toff, vref) can be used for controlling the bridge driver, or can be safely ignored or removed.

## Acknowledgement
This code is mainly based on the work of V. Hunter Adams (see https://vanhunteradams.com/Pico/Steppers/Lorenz.html for more details and complex examples).
