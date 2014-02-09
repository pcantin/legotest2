#include "arduino.h"
#include <Time.h>
#include "TestSequence.h"
#include "freqout.h"
#include <AFMotor.h>
#include <Servo.h>

extern int gUpDwnMotorUp;
extern int gUpDwnMotorDown;
extern int gInOutMotorUp;
extern int gInOutMotorDown;
extern int gModeServoRest;
extern int gModeServoIn;
extern int gIRread;


MotorItem::MotorItem(){
  m_Type = MOTOR_None;
}

MotorItem::~MotorItem(){
}

void
MotorItem::Set(AF_DCMotor *i_motor, int i_rotSensor, ItemType i_item){
  m_Type = MOTOR_DC;
  m_Item = i_item;  
  m_pMotor = i_motor;
  m_rotSensor = i_rotSensor;
}

void
MotorItem::Set(Servo *i_servo, int i_pin, ItemType i_item){
  m_Type = MOTOR_Servo;
  m_Item = i_item;  
  m_pServo = i_servo;
  m_pServo->attach(i_pin);
}

void
MotorItem::CountedTurn(int i_maxTime, int i_dir, int i_StallLevel){
  if(i_maxTime <= 0) return;

  int iterDelay = 2;//ms
  int valueSet[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int valueIter = 0;
  int avgValue = 0;  
  int lastAvgValue = 0;  
  int stalledCount = 0;
  int stalledStop = 1;
  
  if(i_StallLevel == 2) stalledStop = 2;
  if(i_StallLevel == 1) stalledStop = 3;
  
  if(i_dir == 1)
    m_pMotor->run(FORWARD);
  else
    m_pMotor->run(BACKWARD);
    
//  if(400 < i_maxTime) {
//    delay(400);//Let the motor ramp up
//  }

  time_t startTime = millis();
  time_t diffTime = millis() - startTime;
  
  while((diffTime < i_maxTime) && (stalledStop > stalledCount)){
    valueSet[valueIter++] = analogRead(m_rotSensor);
    if(valueIter > 19) valueIter = 0;
    
    lastAvgValue = avgValue;
    avgValue = 0;
    for(int i = 0; i < 20; i++){
      avgValue += valueSet[i];
    }
    
    avgValue /= 20;
/*  Serial.print("time=");      
  Serial.print(diffTime);       
  Serial.print("/");      
  Serial.print(i_maxTime);       
  Serial.print(" avg= ");      
  Serial.print(avgValue);  */     
  
    if (avgValue <= 415 && diffTime > 300){ //If stalled
      stalledCount++;
    } else{
      stalledCount--; //Motor is running
      if(0 > stalledCount) stalledCount = 0;
    }
    
    delay(iterDelay);
    diffTime = millis() - startTime;
  }
  
  m_pMotor->run(RELEASE);
}

SensorItem::SensorItem(){
  m_Type = SENSOR_None;
}

SensorItem::SensorItem(SensorType i_Type){
  m_Type = i_Type;
}
  
SensorItem::~SensorItem(){
}
    

TestSequence::TestSequence(){
}

TestSequence::~TestSequence(){
}

void 
TestSequence::SetMotorUpDwn(AF_DCMotor *i_motor, int i_rotSensor){
  m_MotorUpDwn.Set(i_motor, i_rotSensor, ITEM_updnMotor);
}

void 
TestSequence::SetMotorInOut(AF_DCMotor *i_motor, int i_rotSensor){
  m_MotorInOut.Set(i_motor, i_rotSensor, ITEM_inoutMotor);
}

void 
TestSequence::SetServo(Servo *i_servo, int i_pin){
  m_Servo.Set(i_servo, i_pin, ITEM_liftServo);
}

void
TestSequence::Reset(void){  
  m_MotorUpDwn.GetMotor()->setSpeed(130);
  m_MotorUpDwn.CountedTurn(2000, gUpDwnMotorDown);  

  m_Servo.GetServo()->write(gModeServoRest);
  delay(1000);

  m_MotorInOut.GetMotor()->setSpeed(150);  
  m_MotorInOut.CountedTurn(8000, gInOutMotorDown, 2);  
  
  m_MotorInOut.GetMotor()->setSpeed(120);
  m_MotorInOut.CountedTurn(2000, gInOutMotorUp, 2);  
  m_MotorInOut.CountedTurn(2000, gInOutMotorUp, 2);  
  m_MotorInOut.CountedTurn(65, gInOutMotorDown, 0);  
}

void
TestSequence::Disassemble(void){
  m_MotorInOut.GetMotor()->setSpeed(160);
  m_MotorInOut.CountedTurn(2000, gInOutMotorUp, 2);  
  m_MotorInOut.CountedTurn(2000, gInOutMotorUp, 2);  
  
  m_MotorUpDwn.GetMotor()->setSpeed(160);
  m_MotorUpDwn.CountedTurn(9000, gUpDwnMotorUp, 2);  
  m_MotorUpDwn.GetMotor()->setSpeed(130);
  m_MotorUpDwn.CountedTurn(9000, gUpDwnMotorUp, 1);  

  m_MotorUpDwn.GetMotor()->setSpeed(120);
  m_MotorUpDwn.CountedTurn(8000, gUpDwnMotorDown);  

//  m_MotorInOut.GetMotor()->setSpeed(120);
//  m_MotorInOut.CountedTurn(65, gInOutMotorDown, 0);  
}

void
TestSequence::Assemble(void){
  m_MotorInOut.GetMotor()->setSpeed(150);  
  m_MotorInOut.CountedTurn(8000, gInOutMotorDown, 2);  
  
  m_MotorInOut.GetMotor()->setSpeed(120);
  m_MotorInOut.CountedTurn(2000, gInOutMotorUp, 2);  
  m_MotorInOut.CountedTurn(2000, gInOutMotorUp, 2);  
//  m_MotorInOut.CountedTurn(2000, gInOutMotorUp, 2);  
  m_MotorInOut.CountedTurn(65, gInOutMotorDown, 0);  
}

void
TestSequence::Release(void){
  m_MotorInOut.GetMotor()->setSpeed(120);
  m_MotorInOut.CountedTurn(65, gInOutMotorDown, 0);  
}


int
TestSequence::Check(int i_isOn){
  int res = 0;
  m_Servo.GetServo()->write(gModeServoIn);
  delay(1000);
  
  m_MotorUpDwn.GetMotor()->setSpeed(200);
  m_MotorUpDwn.CountedTurn(2000, gUpDwnMotorUp, 2);  
  delay(200);
  m_MotorUpDwn.GetMotor()->setSpeed(120);
  m_MotorUpDwn.CountedTurn(1000, gUpDwnMotorUp, 1);
  delay(300);  
  m_MotorUpDwn.CountedTurn(50, gUpDwnMotorDown, 0);  


  delay(1000);  
  gIRread = readIRDetector(10, 13);
  delay(10);  
  
  if(i_isOn){
    //Check if LEGO is on
    if(gIRread != 1){
      res = 1;
    } else {
       Serial.println("Check failed!: Brick should be ON");
    }
  } else {
    //Check if LEGO is off
    if(gIRread == 1){
      res = 1;
    } else {
      Serial.println("Check failed!: Brick should be OFF");
    }
  }
  
  if(res == 1){
    m_MotorUpDwn.GetMotor()->setSpeed(120);
    m_MotorUpDwn.CountedTurn(2000, gUpDwnMotorDown);  

    m_Servo.GetServo()->write(gModeServoRest);
    delay(1000);
  }
   

  return res;
}

void
TestSequence::StalledTest(AF_DCMotor *i_motor){
  Serial.println("Start test PAUSE");       
  
  AF_DCMotor * m_pMotor = i_motor;

  Serial.println("RELEASE");       
  m_pMotor->run(RELEASE);
  delay(3000);

  Serial.println("FORWARD - stalled");       
  m_pMotor->run(FORWARD);
//  m_pMotor->run(BACKWARD);
  
  int testIsRunning = 1;
  int mtrSpeed = 130;
  time_t startTime = now();
  time_t diffTime = now() - startTime;
  int valueSet[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int valueIter = 0;
  int avgValue = 0;
  int lastAvgValue = 0;  
  int isRunning = 0;  int upTheSpeed = 0;
  
  while(testIsRunning && (diffTime < 6)){
    valueSet[valueIter++] = analogRead(0);
    if(valueIter > 19) valueIter = 0;
    
    lastAvgValue = avgValue;
    avgValue = 0;
    for(int i = 0; i < 20; i++){
      avgValue += valueSet[i];
    }
    
    avgValue /= 20;
    
    m_pMotor->setSpeed(mtrSpeed);
    
    if(diffTime > 0){ //Let the motor ramp up
        if(upTheSpeed > 20){
          mtrSpeed += 5;
          
          if(mtrSpeed > 255) mtrSpeed = 255;
          
          upTheSpeed = 0;
        }
        upTheSpeed++;
    }

    Serial.print(avgValue);       
    Serial.print(",");      
    Serial.print(mtrSpeed);       
    Serial.print(",");      
    Serial.println(diffTime);       
      
    delay(1);
    diffTime = now() - startTime;
  }   

  Serial.println("RELEASE");       
  m_pMotor->run(RELEASE);
  delay(3000);

  Serial.println("FORWARD - loose");       
  m_pMotor->run(FORWARD);
  
  testIsRunning = 1;
  mtrSpeed = 130;
  startTime = now();
  diffTime = now() - startTime;
  valueIter = 0;
  avgValue = 0;
  upTheSpeed = 0;
  
  while(testIsRunning && (diffTime < 6)){
    valueSet[valueIter++] = analogRead(0);
    if(valueIter > 19) valueIter = 0;
    
    lastAvgValue = avgValue;
    avgValue = 0;
    for(int i = 0; i < 20; i++){
      avgValue += valueSet[i];
    }
    
    avgValue /= 20;
    
    m_pMotor->setSpeed(mtrSpeed);
    
    if(diffTime > 0){ //Let the motor ramp up
        if(upTheSpeed > 20){
          mtrSpeed += 5;

          if(mtrSpeed > 255) mtrSpeed = 255;

          upTheSpeed = 0;
        }
        upTheSpeed++;
    }

    Serial.print(avgValue);       
    Serial.print(",");      
    Serial.print(mtrSpeed);       
    Serial.print(",");      
    Serial.println(diffTime);       
      
    delay(1);
    diffTime = now() - startTime;
  }   
   m_pMotor->run(RELEASE);
   Serial.println("TEST COMPLETED ----------------------");       
}

