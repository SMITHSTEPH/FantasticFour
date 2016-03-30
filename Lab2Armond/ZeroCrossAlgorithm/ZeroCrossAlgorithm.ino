volatile unsigned long firstPulseTime;
volatile unsigned long lastPulseTime;

short numPulses=0;
short inPin=2;
short inTone;
short prevTone;
bool valid=false;
String packets="";

bool interrupt=false;
void isr()
{

  unsigned long now = micros();
  if(valid){prevTone=inTone;}
  if (numPulses == 1){firstPulseTime = now;} //getting the period of the incoming tone
  else{lastPulseTime = now;}
  numPulses++;
  Serial.println(now);
  if(numPulses==2) //one firstPulseTime & lastPulseTime are set for one pulse
  {
    /*if(lastPulseTime-firstPulseTime>=588) //determine whether or not the tone is a 0 or 1
    {
      
      valid=true; //we can assign this tone to prevTone & use this tone to decode the signal
      inTone=0;
      packets+="0";
    } 
    else
    {
      valid=true;
      inTone=1;
      packets+="1";
    }
    //else{valid=false;}*/
    numPulses=0; //reset num pulses so we can get the time again
  }
  //if(valid && (prevTone==inTone)){packets+="1";} //no change in tone
  //else if(valid){packets+="0";}
  
}

void setup()
{
 Serial.begin(19200);    // this is here so that we can print the result
 pinMode(inPin, INPUT);
 attachInterrupt(digitalPinToInterrupt(inPin), isr, RISING);    // enable the interrupt
}
void serialEvent()
{
  while (Serial.available()) 
  {
   char inChar = (char)Serial.read();
   Serial.print("data:");
   Serial.write(inChar);
   Serial.println();
   Serial.print(packets);
  }
}

// Measure the frequency over the specified sample time in milliseconds, returning the frequency in Hz
/*float readFrequency(unsigned int sampleTime)
{
 numPulses = 0;                      // prime the system to start a new reading
 attachInterrupt(digitalPinToInterrupt(inPin), isr, RISING);    // enable the interrupt
 //delay(sampleTime);
 detachInterrupt(digitalPinToInterrupt(inPin));
 return (numPulses < 3) ? 0 : (1000000.0 * (float)(numPulses - 2))/(float)(lastPulseTime - firstPulseTime);
}*/

void loop()
{
  //if(interrupt) Serial.println("TRUE!");
  //int val=digitalRead(2);
  //Serial.println(val);
  //float freq = readFrequency(100);
  //Serial.println(freq);
  //delay(1000);
}
