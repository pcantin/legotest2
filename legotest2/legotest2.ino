#include <AFMotor.h>
#include <Time.h>
#include <Servo.h>
#include "TestSequence.h"
#include <EEPROM.h>
#include "EEPROMAnything.h"

int gOnSwitchPin = 2;
int gOnSwitch = 0;
int gState = 0;
int gTestIsRunning = 0;

AF_DCMotor gUpDwnMotor(3, MOTOR34_8KHZ); 
AF_DCMotor gInOutMotor(4, MOTOR34_8KHZ); 
Servo gModeServo;

int gUpDwnMotorUp = 1;
int gUpDwnMotorDown = 0;
int gInOutMotorUp = 0;
int gInOutMotorDown = 1;
int gModeServoRest = 45;
int gModeServoIn = 90;
int gIRread = 0;

struct saveData_t
{
    long totalIter;
} gSavedData;

long gTotalIter = 0; 

TestSequence gTestObj;

void setup() {
  Serial.begin(115200); 
  Serial.print("$$$");
  Serial.println("SM,0");
  Serial.println("---"); 
    
  pinMode(gOnSwitchPin, INPUT);
  
  gTestObj.SetServo(&gModeServo, 9);
  gTestObj.SetMotorUpDwn(&gUpDwnMotor, 1);
  gTestObj.SetMotorInOut(&gInOutMotor, 0);

//  gTestObj.StalledTest(&gInOutMotor);

  gOnSwitch = digitalRead(gOnSwitchPin);

  gTestObj.Reset();
  
  if(gTestObj.Check(1)){
    gTestIsRunning = 1; //We are ready to begin the test
  }  
  
  if(gOnSwitch == LOW){
    //Force reset the count to 0
    gSavedData.totalIter = 0;
    EEPROM_writeAnything(0, gSavedData);
  } else {
    //Continue the count to the last total
    if(0 == EEPROM_readAnything(0, gSavedData)){
      //This case should neverr happen. If it does we reset the total to 0
      gSavedData.totalIter = 0;
    }
  }
}

void loop() {
  gOnSwitch = digitalRead(gOnSwitchPin);
  
  if(gOnSwitch == LOW && 1 == gTestIsRunning){
    gTestObj.Disassemble();
    gTestIsRunning = gTestObj.Check(0);
    gTestObj.Release();
    
    if(1 == gTestIsRunning){
      gTestObj.Assemble();
      gTestIsRunning = gTestObj.Check(1);
      
      if(1 == gTestIsRunning){
        gSavedData.totalIter++;
      }
    }
    
    if(1 != gTestIsRunning){
      EEPROM_writeAnything(0, gSavedData);
      gTestObj.Reset();
    }
    
    Serial.print("Total: ");
    Serial.println(gSavedData.totalIter);
  }
  
  if(1 == gTestIsRunning){  
    delay(100); 
  }else{
    delay(1000); 
    Serial.print("Stoped total: ");
    Serial.println(gSavedData.totalIter);  }
}
