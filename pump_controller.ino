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

#define HYST 6 // hysteresis, about +&- 2.5°C (total 5°C)
#define VAL_0_DEG 430
#define VAL_100_DEG 625

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
int getState(uint16_t sensor1Val, uint16_t sensor2Val, uint16_t treshVal)
{
  // state doesn't change if tresh-hyst < t2 < tresh+hyst
  if ((sensor1Val > VAL_100_DEG) || (sensor2Val > VAL_100_DEG) ||
      (sensor1Val < VAL_0_DEG)   || (sensor2Val < VAL_0_DEG))
    return NO_SENSOR;

  if (sensor1Val < treshVal-HYST)
    return TEMP_UNRCH;

  if (sensor2Val < (treshVal-HYST))
    return PUMPING;

  return TEMP_OK;
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

static float real_temp(uint16_t input) {
	return (input - VAL_0_DEG) * 100.f / (VAL_100_DEG - VAL_0_DEG);
}
    
//-----------------------------------------------------------------
void loop(){
  uint16_t sensor1Val = analogRead(sensor1Pin);
  uint16_t sensor2Val = analogRead(sensor2Pin); 
  uint16_t potentioVal = analogRead(potentioPin);  
  
  // converts to the sensors scale
  uint16_t treshVal = potentioVal * (VAL_100_DEG - VAL_0_DEG) / 1024 + VAL_0_DEG;

  sendValue("Sensor1", real_temp(sensor1Val));
  sendValue("Sensor2", real_temp(sensor2Val));
  sendValue("Treshold", real_temp(treshVal));
  
  int state = getState(sensor1Val, sensor2Val, treshVal);

  sendValue("state", state);

  updateState(state);

  delay(200);
}
