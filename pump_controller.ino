/*-----------------------------------------------------------------
Name: pump_controller.ino
Date: 23.06.2017
Author: Florent Knecht
Hardware: Arduino Nano. The temperature sensors are resistors in
 series with resistors having similar values.
Description: the controller pumps water from a hot water tank #1 to
 an other tank #2 until the desired temperature is reached.
------------------------------------------------------------------*/

#include "pump_control.h"

const int sensor1Pin = A1; // temperature sensor from tank #1 ; connected on left side
const int sensor2Pin = A2; // temperature sensor from tank #2 ; connected on right side
const int potentioPin = A3; // potentiometer, telling the desired temperature
const int relaisPin = 2; // relais activationg the pump
const int powerLedPin = 3; 
const int relaisLedPin = 4;
const int tMaxLedPin = 5;

//-----------------------------------------------------------------
void setup(){
  Serial.begin(9600);
  
  for(int pinNumber = 2; pinNumber < 6; pinNumber++){
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, LOW);
  }
}

//-----------------------------------------------------------------
void blinkPwrLed(void)
{
  static int ledState=0;
  ledState = (ledState==0)? 1 : 0;  
  digitalWrite(powerLedPin, ledState);
}

//-----------------------------------------------------------------
void setLed(int powerLed, int relaisLed, int tMaxLed)
{
  digitalWrite(powerLedPin, powerLed);
  digitalWrite(relaisLedPin, relaisLed);
  digitalWrite(tMaxLedPin, tMaxLed);
}    

//-----------------------------------------------------------------
// send to arduino serial console for debugging
void sendValue(const char* label, float value)
{
  Serial.print(label);
  Serial.print(" :");
  Serial.println(value);
}

//-----------------------------------------------------------------
void updateState(pump_state_t state)
{
  switch(state)
  {
    case SENSOR_ERROR:
      setLed(0,0,0);
      blinkPwrLed();
      digitalWrite(relaisPin, LOW);
      break;
    case STATE_TEMP_LOW:
      setLed(1,0,0);
      digitalWrite(relaisPin, LOW);
      break;
    case STATE_PUMPING:
      setLed(1,1,0);
      digitalWrite(relaisPin, HIGH);
      break;
    case STATE_TEMP_OK:
      setLed(1,0,1);
      digitalWrite(relaisPin, LOW);
      break;
  }
}
    
//-----------------------------------------------------------------
void loop(){
  static pump_state_t state = STATE_TEMP_LOW;
  uint16_t sensor1Val = analogRead(sensor1Pin);
  uint16_t sensor2Val = analogRead(sensor2Pin);
  uint16_t potentioVal = analogRead(potentioPin);
  
  // converts to the sensors scale
  uint16_t treshVal = (int) potentioVal * (VAL_100_DEG - VAL_0_DEG) / 1024 + VAL_0_DEG;

  sendValue("Sensor1", real_temp(sensor1Val));
  sendValue("Sensor2", real_temp(sensor2Val));
  sendValue("Treshold", real_temp(treshVal));
  
  state = get_pump_state(state, sensor1Val, treshVal, sensor2Val);

  sendValue("state", state);

  updateState(state);

  delay(200);
}
