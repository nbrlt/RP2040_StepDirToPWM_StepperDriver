# RP2040 Unipolar StepDir Driver
This is a simple arduino code to use the RP2040 as logic to convert a Step/Dir interface into 4 signals used for PWM type bridge drivers such as the DRV8803 or DRV8932. 

The logic is coded with 1/2 steps commonly used with unipolar steppers, but it could be easily transformed into something else (even into bipolar logic) by modifying the pulse sequence.

## Usage
The RP2040 can control up to 2 motors. When you define the motor port, 5 consecutive pins will be allocated to it, the last one being the step input, the rest being the control signal for the bridge driver. The dir pin is used through interrupts to control the direction of the motor.

The extra pins defined in the sketch (enable, toff, vref) can be used for controlling the bridge driver, or can be safely ignored or removed.

