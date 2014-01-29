//TestSequence.h


#ifndef TESTSEQUENCE_H
#define TESTSEQUENCE_H

enum MotorType { 
  MOTOR_None,
  MOTOR_DC,
  MOTOR_Servo,
};

enum SensorType { 
  SENSOR_None,
  SENSOR_IR,
  SENSOR_Switch,
};

enum ItemType { 
  ITEM_updnMotor,
  ITEM_inoutMotor,
  ITEM_liftServo,
  ITEM_irSensor,
};

class AF_DCMotor;
class Servo;

class MotorItem {
public:
  MotorItem();
  ~MotorItem();

  void Set(AF_DCMotor *i_motor, int i_rotSensor, ItemType i_item);
  void Set(Servo *i_servo, int i_pin, ItemType i_item);

  AF_DCMotor* GetMotor(void) {return m_pMotor;};
  Servo* GetServo(void) {return m_pServo;};
  
  void CountedTurn(int i_maxTime, int i_dir, int i_StallLevel = 0);

private:
  MotorType m_Type;
  ItemType  m_Item;
  
  AF_DCMotor *m_pMotor;
  int        m_rotSensor;
  
  Servo      *m_pServo;
};

class SensorItem {
public:
  SensorItem();
  SensorItem(SensorType i_Type);
  ~SensorItem();
  
  int GetValue(void) { return m_value; };

private:
  SensorType m_Type;
  int        m_value;
};



class TestSequence {
public:
  TestSequence();
  ~TestSequence();
  
  void SetMotorUpDwn(AF_DCMotor *i_motor, int i_rotSensor);
  void SetMotorInOut(AF_DCMotor *i_motor, int i_rotSensor);
  void SetServo(Servo *i_servo, int i_pin);
  
  void Reset(void);
  void Disassemble(void);
  void Assemble(void);
  void Release(void);
  int  Check(int i_isOn);
  void StalledTest(AF_DCMotor *i_motor);

private:
  MotorItem m_MotorUpDwn;
  MotorItem m_MotorInOut;
  MotorItem m_Servo;
};


#endif
