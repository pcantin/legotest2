#include <AFMotor.h>
#include <Time.h>
#include <Servo.h>
#include "TestSequence.h"

int gOnSwitchPin = 2;
int gOnSwitch = 0;
int gState = 0;
int gTestIsRunning = 0;

AF_DCMotor gUpDwnMotor(3, MOTOR34_8KHZ); 
AF_DCMotor gInOutMotor(2, MOTOR34_8KHZ); 
Servo gModeServo;

int gUpDwnMotorUp = 1;
int gUpDwnMotorDown = 0;
int gInOutMotorUp = 0;
int gInOutMotorDown = 1;
int gModeServoRest = 45;
int gModeServoIn = 90;
int gIRread = 0;

TestSequence gTestObj;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("LEGO Test Machine #2");
  
  pinMode(gOnSwitchPin, INPUT);
  
  gTestObj.SetServo(&gModeServo, 9);
  gTestObj.SetMotorUpDwn(&gUpDwnMotor, 1);
  gTestObj.SetMotorInOut(&gInOutMotor, 0);

//  gTestObj.StalledTest(&gInOutMotor);
  gTestObj.Reset();
  
  if(gTestObj.Check(1)){
    gTestIsRunning = 1;
  }

  gOnSwitch = digitalRead(gOnSwitchPin);

  if(gOnSwitch == LOW){
    Serial.println("Switch is ON");
  } else {
    Serial.println("Switch is Off");
  }
}

void loop() {
  gOnSwitch = digitalRead(gOnSwitchPin);
  
  if(gOnSwitch == LOW && 1 == gTestIsRunning){
    gTestObj.Disassemble();
    gTestIsRunning = gTestObj.Check(0);
    
    if(1 == gTestIsRunning){
      gTestObj.Assemble();
      gTestIsRunning = gTestObj.Check(1);
    }
  }
  
  delay(100); 
}
