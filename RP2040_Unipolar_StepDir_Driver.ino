#include <RP2040_StepDir_motorlogic.h>


int motorpin1 = 0;// 5 pins used by PIO code
int dirpin1 = 5;
int motor1_en = 8;
int motor1_toff = 6;
int motor1_vref = 7;

int motorpin2 = 9;// 5 pins used by PIO code
int dirpin2 = 14;
int motor2_en = 27;
int motor2_toff = 15;
int motor2_vref = 26;

//int motorpin3 = 9;// 5 pins used by PIO code
//int dirpin3 = 14;

//int motorpin4 = 9;// 5 pins used by PIO code
//int dirpin4 = 14;

void setup() {

  pinMode(dirpin1, INPUT);
  pinMode(dirpin2, INPUT);
  //pinMode(dirpin3, INPUT);
  //pinMode(dirpin4, INPUT);

  pinMode(motor1_en, INPUT); // Input if managed by other
  pinMode(motor1_toff, OUTPUT);
  pinMode(motor1_vref, INPUT); // Input if managed by other

  pinMode(motor2_en, INPUT); // Input if managed by other
  pinMode(motor2_toff, OUTPUT); 
  pinMode(motor2_vref, INPUT); // Input if managed by other

  //digitalWrite(motor1_en,HIGH); 
  //digitalWrite(motor2_en,HIGH);

  digitalWrite(motor1_toff,LOW); // LOW 7us, HIGH 16us, INPUT with 330k to GND 32us (floating 24us)
  digitalWrite(motor2_toff,LOW);

  setupMotor1(motorpin1); // 5 pins used by PIO code
  setupMotor2(motorpin2); // 5 pins used by PIO code
  //setupMotor3(motorpin3); // 5 pins used by PIO code
  //setupMotor4(motorpin4); // 5 pins used by PIO code

  motor1_dir_ISR();
  motor2_dir_ISR();
  //motor3_dir_ISR()
  //motor4_dir_ISR()

  
  attachInterrupt(digitalPinToInterrupt(dirpin1), motor1_dir_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(dirpin2), motor2_dir_ISR, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(dirpin3), motor3_dir_ISR, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(dirpin4), motor4_dir_ISR, CHANGE);


}

void motor1_dir_ISR() {
  if (digitalRead(dirpin1)==0){
    SET_DIRECTION_MOTOR_1(1);
  } else {
    SET_DIRECTION_MOTOR_1(2);
  }
}
void motor2_dir_ISR() {
    if (digitalRead(dirpin2)==0){
    SET_DIRECTION_MOTOR_2(1);
  } else {
    SET_DIRECTION_MOTOR_2(2);
  }
}
// void motor3_dir_ISR() {
//     if (digitalRead(dirpin3)==0){
//     SET_DIRECTION_MOTOR_3(1);
//   } else {
//     SET_DIRECTION_MOTOR_3(2);
//   }
// }
// void motor4_dir_ISR() {
//     if (digitalRead(dirpin4)==0){
//     SET_DIRECTION_MOTOR_4(1);
//   } else {
//     SET_DIRECTION_MOTOR_4(2);
//   }
// }

void loop() {
 
}
