
#include "QueueList.h"
const short LEDPins[] = {5,6,8,9,11,12,13}; //need to have 7 LEDs
const short LEDS_NUM =7;
bool pcReady = false;
const int tempPin=0; //(analog) connected to A0
QueueList <short> queue;
short temperature=0;
void setup()
{
  while(millis()<=300000)
  {
    short temperature = readTempSensor();
    storeTempData(temperature);
  }
  blinkLEDS();
  while(!pcReady){} //wait until the PC is ready to send all of the data
  sendData();
}
void loop(){} //don't need to use a loop
void blinkLEDS()
{
   writeAllLEDS(0);
   delay(100);
   writeAllLEDS(1);
   delay(100);
}
void serialEvent()
{
  short pcData = Serial.readString().toInt();
  if(pcData==0){pcReady=true;}
  else if(pcData==1){pcReady=false;}
}
void sendData()
{
  while(!queue.isEmpty() && pcReady) {Serial.println(queue.pop());}
}
void storeTempData(short temp){queue.push(temp);}
short readTempSensor()
{
  short temperature = analogRead(tempPin); //casting input to integer
  temperature = temperature * 0.48828125;
  return temperature;
}
 void writeAllLEDS(bool mode)
 {
    for(int i = 0; i < LEDS_NUM; i++){digitalWrite(LEDPins[i], mode);}
 }
