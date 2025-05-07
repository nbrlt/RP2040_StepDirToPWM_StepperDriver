# RP2040 StepDir to 4wire-PWM stepper logic
This is a simple arduino code to use the RP2040 as logic to convert a Step/Dir interface into 4 signals used for PWM type drivers such as the ULN2003 or DRV8932. 

The logic is coded with 1/2 steps commonly used with unipolar steppers, but it could be easily transformed into something else (even into bipolar logic) by modifying the pulse sequence.

## Usage
The RP2040 can control up to 2 motors. When you define the motor port, 4 consecutive pins will be allocated to it. Conenct those 4 pins to the PWM inputs of your bridge driver. The step and dir input control the movements of the motor.

The extra pins defined in the sketch (enable, toff, vref) can be used for controlling the bridge driver, or can be safely ignored or removed.

The RP2040 uses 3V3 logic.

