
#include "TimerOne.h" //16 bit timer --> should be no timer overflows for this purpose
//http://playground.arduino.cc/Code/Timer1

/*globals*/

/*for zero-crossing algorithm*/
short inTone; //tone coming in currently 
short prevTone=0; //previous tone that came in (need to keep track of in order to decode)
bool startTrans=false; //set to true the first time transmission begins
//boolean TMR1_flag;
/*for arduino*/
short inPin=2; //using digital pin 8
String packets="";
int i=0;
int val=3;

void setup() 
{
  pinMode(inPin, INPUT); 
  //digitalWrite(inPin,HIGH);  // enable pullup resistor
  attachInterrupt(digitalPinToInterrupt(inPin), determineFrequency, CHANGE);
  Serial.begin(9600); //configure baudrate for serial port
  Timer1.initialize(294); //argument is period in microseconds --> 588/2
  //if more than two ticks have passed, then it beyond the 1700 Hz threshold
}

void loop() 
{
    if(inTone==0 &&  prevTone==0)
    {
    }
}
/**function will display HDLC packet**/
void displayPacket(void)
{
}

void determineFrequency() //part of zero crossing algorithm
{
  //prevTone=inTone; //get prevTone
  Timer1.restart();
  unsigned long curTime=0;
  inTone=HIGH;
  while(curTime<588){curTime=Timer1.read();} //doesn't really account for noise
  inTone=LOW;
}
/**function will collect and decode signal**/

void zeroCrossingAlgorithm(void)
{

  
}
/**
 * this function calcutates the CRC
 * & then determines wether or not the packet transmitted
 * successfully
 * 
 * returns true if there was an error
 * returns false if no error
 **/
bool isPacketError(void)
{
}
/*void serialEvent()
{
  while (Serial.available()) 
  {
   char inChar = (char)Serial.read();
   Serial.print("data:");
   Serial.write(inChar);
   Serial.println();
   Serial.print(packets);
  }
}*/
//might not need this
/*void serialEvent() {
  while (Serial.available()) {

    int inTone = (int)Serial.read();
    if(inTone == 0){
        if(prevTone == 0){
          if(TMR1_flag == true){
            //set TMR1_flag to 1 bit periods
            //clear TMR1_flag
            //return 1
          }
      }
      else{
        prevTone = 0;
        //set TMR1_flag to 1.5 bit periods
        //clear TMR1_flag
        //return 0
      }
    }
    else{
      if(prevTone == 0){
        prevTone = 1;
        //set TMR1_flag to 1.5 bit periods
        //clear TMR1_flag
        //return 0
      }
      else{
        if(TMR1_flag == true){
          //set TMR1_flag to 1 bit periods
          //clear TMR1_flag
          //return 1
        }
        else{
          //jump back to start to check if tone is equal to 0
        }
      }
    }
}*/

