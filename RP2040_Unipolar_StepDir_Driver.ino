// Unipolar motor driver controller
// Nicolas Bourlet, 2025
// https://github.com/nbrlt

#include <pico/stdlib.h>
#include <stdio.h>
#include <map>
#include <string>

int motorpin1 = 0; // 4 consecutive pins used for PWM on controller
int motor1_step = 4; //Input step signal
int dirpin1 = 5; // Input dir signal
int motor1_en = 8; //Enable pin of driver
int motor1_toff = 6; // toff pin of driver
int motor1_vref = 7; // vref pin of driver

int mask_motor1=15<<motorpin1; // mask for direct gpio writing

int motorpin2 = 9;// 4 consecutive pins used for PWM on controller
int motor2_step = 13;//Input step signal
int dirpin2 = 14;// Input dir signal
int motor2_en = 27;//Enable pin of driver
int motor2_toff = 15;// toff pin of driver
int motor2_vref = 26;// vref pin of driver
int mask_motor2=15<<motorpin2; // mask for direct gpio writing

//Indexes for sequencing
int motor1_ind = 0;
int motor2_ind = 0;

// Initial direction
int motor1_dir = 1;
int motor2_dir = 1;

//Unipolar sequencing with 1/2 microstepping
unsigned char pulse_sequence_forward [8]  = {0x9, 0x8, 0xc, 0x4, 0x6, 0x2, 0x3, 0x1} ;

//Temp storing integer
int value;

void setup() {
  // Pins mode setup
  for (int pin = motorpin1; pin < motorpin1 + 4; pin++) {
      pinMode(pin, OUTPUT);
  }
  for (int pin = motorpin2; pin < motorpin2 + 4; pin++) {
      pinMode(pin, OUTPUT);
  }
  pinMode(motor1_step, INPUT);
  pinMode(motor2_step, INPUT);
  pinMode(dirpin1, INPUT);
  pinMode(dirpin2, INPUT);

  pinMode(motor1_en, INPUT); // Input if managed by other controller
  //pinMode(motor1_en, OUTPUT); // OUTPUT, check driver datasheet for value

  //pinMode(motor1_toff, INPUT); // Input if managed by other controller
  pinMode(motor1_toff, OUTPUT); // OUTPUT, check driver datasheet for value
 
  pinMode(motor1_vref, INPUT); // Input if managed by other controller
  //pinMode(motor1_vref, OUTPUT); // OUTPUT, check driver datasheet for value


  pinMode(motor2_en, INPUT); // Input if managed by other
  pinMode(motor2_toff, OUTPUT); 
  pinMode(motor2_vref, INPUT); // Input if managed by other

  // Set toff values
  digitalWrite(motor1_toff,LOW); // LOW 7us, HIGH 16us, INPUT with 330k to GND 32us (floating 24us)
  digitalWrite(motor2_toff,LOW);
  // Set enable and vref pins here if needed

  // Manually parse dir value
  motor1_dir_ISR();
  motor2_dir_ISR();

  // initialise the PWM outputs
  value = pulse_sequence_forward[motor1_ind] << motorpin1;
  gpio_put_masked(mask_motor1,value);
  value = pulse_sequence_forward[motor2_ind] << motorpin2;
  gpio_put_masked(mask_motor2,value);

  // attach interrupts
  attachInterrupt(digitalPinToInterrupt(dirpin1), motor1_dir_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(dirpin2), motor2_dir_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(motor1_step), motor1_step_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(motor2_step), motor2_step_ISR, RISING);

}

void motor1_step_ISR() {
  motor1_ind = motor1_ind+motor1_dir;
  if (motor1_ind >7) {
    motor1_ind=0;
  }
  if (motor1_ind <0) {
    motor1_ind=7;
  }
  value = pulse_sequence_forward[motor1_ind] << motorpin1;
  gpio_put_masked(mask_motor1,value);
}
void motor2_step_ISR() {
  motor2_ind = motor2_ind+motor2_dir;
  if (motor2_ind >7) {
    motor2_ind=0;
  }
  if (motor2_ind <0) {
    motor2_ind=7;
  }
  value = pulse_sequence_forward[motor2_ind] << motorpin2;
  gpio_put_masked(mask_motor2,value);
}

void motor1_dir_ISR() {
  if (digitalRead(dirpin1)==0){
    motor1_dir = 1;
  } else {
    motor1_dir = -1;
  }
}
void motor2_dir_ISR() {
  if (digitalRead(dirpin2)==0){
    motor2_dir = 1;
  } else {
    motor2_dir = -1;
  }
}

void loop() {
 
}
