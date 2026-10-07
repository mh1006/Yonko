#include <Servo.h>

const int btn1Pin = 7;
const int btn2Pin = 8;
const int btn3Pin = 12;
const int servoPin = 9;

Servo myServo;
int leavesAngle = 90; 

void setupLeaves() {
  pinMode(btn1Pin, INPUT_PULLUP);
  pinMode(btn2Pin, INPUT_PULLUP);
  pinMode(btn3Pin, INPUT_PULLUP);
  myServo.attach(servoPin);
  myServo.write(leavesAngle);
}

void updateLeaves() {
  if (digitalRead(btn1Pin) == LOW) {
    leavesAngle = 10;
    myServo.write(leavesAngle);
  } 
  else if (digitalRead(btn2Pin) == LOW) {
    leavesAngle = 90;
    myServo.write(leavesAngle);
  } 
  else if (digitalRead(btn3Pin) == LOW) {
    leavesAngle = 170;
    myServo.write(leavesAngle);
  }
}