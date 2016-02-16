/**
 * Add thirdbox code description
 */
#include "QueueList.h"
#include "OneWire.h"
#include "DallasTemperature.h"
//pins
const short LEDPins[] = {5,6,8,9,11,12,13}; //need to have 7 LEDs
const short LEDS_NUM =7;
const short buttonPin=3; //don't worry about this button yet
const int tempPin=2; //(digital pin) for ds18b20
//const int lmdz35Pin=0 //(analog pin A0) for lm35dz pin
//globals
unsigned long time;
short temperature=0;
bool pc = false; //change to false later
bool unpluggedSensor=false;
QueueList <short> queue;

const short testLEDPin=4;


OneWire oneWire(tempPin); //intializing appropriate references to use the ds18b20
DallasTemperature tempSensor(&oneWire);

void setup() 
{
  for(int i=0; i<LEDS_NUM; i++) { pinMode(LEDPins[i], OUTPUT);} //configuring LED pins
  pinMode(tempPin, INPUT_PULLUP); //configure temp sensor input
  pinMode (buttonPin, INPUT);
  Serial.begin(9600); //for serial communication with serial monitor
  //Serial1.begin(9600); //for serial communication with PC
  attachInterrupt(digitalPinToInterrupt(buttonPin), fastTempDisplay, LOW); //display temp when the button is pressed
  attachInterrupt(digitalPinToInterrupt(buttonPin), clearLEDS, HIGH); //turn off all LEDS when button is released
  //tempSensor.begin();
  //Serial.println("in setup");

  pinMode (testLEDPin, OUTPUT);
  digitalWrite(testLEDPin, HIGH);

  //digitalWrite(testLEDPin, HIGH); //for testing
}
void loop() 
{ 
    //digitalWrite(testLEDPin, HIGH); //for testing
    //temperature=(short)readTempSensor();
    //short temp = unpluggedSensor ? 1000 : temperature; //send 1000 to PC if temperature sensor becomes unplugged
    //sendData(10);
    //delay(500);
    //displayTempTest(temp);
  /*if(pc)
  { 
    //temperature=(short)readTempSensor();
    //short temp = unpluggedSensor ? 1000 : temperature; //send 1000 to PC if temperature sensor becomes unplugged
    sendData(10);
    //displayTempTest(temp);
  }
  //else if (!unpluggedSensor){storeTempData(temperature);}
  //else{errorDisplay();} //blink LEDS if error
  delay(500);*/
}
void clearLEDS(){writeAllLEDS(0);}
/**
 * ARMOND & JOE
 * Interrupt triggered function
 * Called when data is available. Use Serial.read() to capture this data.
 * Here are the messages the arduino could receive from the PC
 *  0 == PC ready
 *  1 == push third box button 
 *  2 == release third box button
 *  3 == PC no longer ready
 */
void serialEvent() //received data on serial data from the PC
{
  //digitalWrite(testLEDPin, HIGH);
  //if(digitalRead(13)!=HIGH){digitalWrite(13, HIGH);}
  int pcData = Serial.parseInt();
  if(pcData>=0){digitalWrite(testLEDPin, HIGH);}
  Serial.println("interrupt data is: " + String(pcData, DEC)); //for testing
  if(pcData==0){pc=true;}
  //else if(pcData==1){fastTempDisplay();}
  //else if(pcData==2){writeAllLEDS(0);}
  else if(pcData==3){pc=false;}
}
void sendData(short data) //casting to a short b/c this information can be represented in 7 bits
{
  /*if(!queue.isEmpty()) //if the queue is not empty flush the queue
  {
    for(int i=0; i<queue.count(); i++){Serial.println(queue.pop());}//sending the temperature
  }*/
  Serial.println(data); //sending the data to the PC
}
/**
 * This function stores the temperature into a queue if it the PC is not available
 * We only was to store 300 seconds worth of data
 */
void storeTempData(short temp)
{
  //need to figure out how to effectively store the data
  time = millis(); 
  //this does not handle the rollover
  if(time>=300000){queue.pop();} //remove one node for every node added
  queue.push(temp);
}
/**
 * This function reads in the analog input and converts this input
 * to a temperature (in degrees celsius) by using this ADC formula:
 * temperature = (reference voltage/resolution) * (analog voltage measured) * 100
 * temperature = (analog voltage measured) * 100 * (5.00 V/1024)
 * temperature = (analog voltage measured) * 0.48828125
 */
 /* //LM35DZ
short readTempSensor()
{
  short temperature = analogRead(tempPin); //casting input to integer
  if(temperature == 0){unpluggedSensor=true;}
  else if(unpluggedSensor){unpluggedSensor=false;}
  temperature = temperature * 0.48828125;
  return temperature;
}*/
float readTempSensor()
{
  tempSensor.requestTemperatures(); // Send the command to get temperatures
  return tempSensor.getTempCByIndex(0);
}
 /**
  * This functin lights up the LED display quickly
  */
 void fastTempDisplay()
 {
    int j=6;
    for(int i=0; i<LEDS_NUM; i++)
    {
      digitalWrite(LEDPins[j], (0x01<<i)&((byte)temperature)); //better way to do this
      j--;
    }
 }
 /**
  * function flashes LEDS on and off
  */
 void errorDisplay()
 {
   writeAllLEDS(0);
   delay(100);
   writeAllLEDS(1);
   delay(100);
 }
 /**
  * this function turns off all of the LEDS after the user lets go of the button 
  */
 void writeAllLEDS(bool mode)
 {
    for(int i = 0; i < LEDS_NUM; i++){digitalWrite(LEDPins[i], mode);}
 }
 /**
 * This function prompts a user to enter temperature values, so that this program can be debugged
 * without having a temperature sensor
 */
int generateTempTest()
{
  Serial.print("enter temp in degrees C: ");
  while (Serial.available() <= 0){} //get the number of bytes available for reading from the serial port
  int incomingTemp = Serial.readString().toInt(); // read the incoming string from the user and converts it an integer
  Serial.println(); 
  return incomingTemp;
 }
 /**
 * This function displays the temperature (in degrees C) to the serial monitor. 
 * This function is for TESTING PURPOSES ONLY
 */
void displayTempTest(float temperature)
{
  Serial.print("Temperature = "); //prints out the temperature to the serial prompt
  Serial.print(String(temperature));
  Serial.println("*C");
}
/**
 * This function converts the integer temperature into a bit String. 
 * -- if the third box is too slow revise this function  --> for testing
 */
String intToBitString(int temperature)
{
  if(temperature <= 63 || temperature >= -64) //possible range of values the the 7 LEDs can display
  {
    int startingIndex=0;
    String wholeByte=String((byte)temperature, BIN); //converting the temperature to a bit string; the size of the bit string depends on where the last 1 is ==> ie if the MSB is 1,
                                         //then the size of the bit string is 8
    if(wholeByte.length()==8){return wholeByte.substring(1, 8);} //return the last 7 bits
    else
    {
      int newLength=7-wholeByte.length();
      for(int i = 0; i < newLength; i++){wholeByte= "0" + wholeByte;} //adding the extras 0s
      return wholeByte;
    }
  }
  else{return "error";} //trying to display a temp val it can't
}

