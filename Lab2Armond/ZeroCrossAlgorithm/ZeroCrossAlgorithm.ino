
#include "TimerOne.h" //16 bit timer --> should be no timer overflows for this purpose
//http://playground.arduino.cc/Code/Timer1

/*globals*/

/*for zero-crossing algorithm*/
short inTone; //tone coming in currently
short prevTone; //previous tone that came in (need to keep track of in order to decode)
boolean TMR1_flag;
/*for arduino*/
short inPin=0; //signal will come in on this pin -> analog or digital pin?


void setup() 
{
  pinMode(inPin, INPUT); 
  attachInterrupt(digitalPinToInterrupt(inPin), determineFrequency, CHANGE);
  Serial.begin(9600); //configure baudrate for serial port
}

void loop() 
{
  zeroCrossingAlgorithm();
  
}
/**function will display HDLC packet**/
void displayPacket(void)
{
}

void determineFrequency(void) //part of zero crossing algorithm
{
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

//might not need this
void serialEvent() {
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
  }
}
