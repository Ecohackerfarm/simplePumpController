/*-----------------------------------------------------------------
Name: pump_controller.ino
Date: 23.06.2017
Author: Florent Knecht
Hardware: Arduino Nano. The temperature sensors are resistors in
 series with resistors having similar values.
Description: the controller pumps water from a hot water tank #1 to
 an other tank #2 until the desired temperature is reached.
------------------------------------------------------------------*/


#define NO_SENSOR 0 // at least one sensor is not connected
#define TEMP_UNRCH 1 // starts when T1 < T_tresh (temperature unreachable)
#define TEMP_OK 2 // starts when T2 > T_tresh+hyst
#define PUMPING 3 // starts when T2 < T_tresh-hyst

#define HYST 0.005 // hysteresis, about +&- 2.5°C (total 5°C)
#define VAL_0_DEG 0.42
#define VAL_100_DEG 0.61

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
void sendValue(char* label, float value)
{
  Serial.print(label);
  Serial.print(" :");
  Serial.println(value);
}

//-----------------------------------------------------------------
int getState(float sensor1Val, float sensor2Val, float treshVal)
{
  static int state = TEMP_OK;

  // state doesn't change if tresh-hyst < t2 < tresh+hyst
  if((sensor1Val > VAL_100_DEG) || (sensor2Val > VAL_100_DEG) || 
     (sensor1Val < VAL_0_DEG) || (sensor2Val < VAL_0_DEG))
    state = NO_SENSOR;
  else if(sensor1Val < treshVal-HYST)
    state = TEMP_UNRCH;
  else if(sensor2Val < (treshVal-HYST)) 
    state = PUMPING;
  else if(sensor2Val > (treshVal+HYST))
    state = TEMP_OK;
    
  return state;
}

//-----------------------------------------------------------------
void updateState(int state)
{
  switch(state)
  {
    case NO_SENSOR:
      setLed(0,0,0);
      blinkPwrLed();
      digitalWrite(relaisPin, LOW);
      break;
    case TEMP_UNRCH:
      setLed(1,0,0);
      digitalWrite(relaisPin, LOW);
      break;
    case PUMPING: 
      setLed(1,1,0);
      digitalWrite(relaisPin, HIGH);
      break;
    case TEMP_OK:
      setLed(1,0,1);
      digitalWrite(relaisPin, LOW);
      break;
  }
}
    
//-----------------------------------------------------------------
void loop(){
  float sensor1Val = analogRead(sensor1Pin)/1024.0;
  float sensor2Val = analogRead(sensor2Pin)/1024.0;  
  float potentioVal = analogRead(potentioPin)/1024.0;  
  
  // converts to the sensors scale
  float treshVal = potentioVal*(VAL_100_DEG-VAL_0_DEG) + VAL_0_DEG; 
  
  sendValue("Sensor1",sensor1Val);
  sendValue("Sensor2",sensor2Val);
  sendValue("Treshold",treshVal);
  
  int state = getState(sensor1Val, sensor2Val, treshVal);
    
  updateState(state);

  delay(200);
  
}
