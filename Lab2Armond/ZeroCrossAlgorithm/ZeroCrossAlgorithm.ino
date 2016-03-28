
/*#include "TimerOne.h" //16 bit timer --> should be no timer overflows for this purpose
//http://playground.arduino.cc/Code/Timer1


int inTone; //tone coming in currently 
int prevTone; //previous tone that came in (need to keep track of in order to decode)
bool valid=false;

short inPin=2; //using digital pin 8
String packets="";
int i=0;
bool startFlag=true;
unsigned long  timer=0;

void setup() 
{
  pinMode(inPin, INPUT); 
  //digitalWrite(inPin,HIGH);  // enable pullup resistor
  attachInterrupt(digitalPinToInterrupt(inPin), zeroCrossingAlgorithm, RISING);
  Serial.begin(9600); //configure baudrate for serial port
  Timer1.initialize(294); //argument is period in microseconds --> 588/2
  //if more than two ticks have passed, then it beyond the 1700 Hz threshold
}

void loop() 
{
   
}
/**function will display HDLC packet**/
void displayPacket(void)
{
}

/*void zeroCrossingAlgorithm() //part of zero crossing algorithm
{
  prevTone=inTone; //get prevTone
  Timer1.restart();
  unsigned long curTime=0;
  inTone=HIGH;
  while(curTime<588){curTime=Timer1.read();} //doesn't really account for noise
  inTone=LOW;
  
}*/
/*

void zeroCrossingAlgorithm(void)
{
  
  timer=Timer1.read();
  Serial.println(timer);
  if(valid) prevTone=inTone;
  if(timer==0 && startFlag) {
    Serial.println("starting timer");
    startFlag=false;
    Timer1.start(); }//first time
  else
  {
    if(timer>588) //1200 HZ
    {
      inTone=LOW;
      valid=true;
    }
    else if(timer>430) //2200 HZ
    {
      inTone=HIGH;
      valid=true;
    }
    else
    {
      valid=false;
    }
    Timer1.restart();
  }
  if(valid && (inTone==0 && prevTone==0) || (inTone==1 && prevTone==1)) //no change
  {
    i++;
    packets+="1"; //roll one onto buffer
  }
  else if(valid) //and there is a change
  {
    i++;
    packets+="0";
  }
}
/**
 * this function calcutates the CRC
 * & then determines wether or not the packet transmitted
 * successfully
 * 
 * returns true if there was an error
 * returns false if no error
 **/
 /*
void serialEvent()
{
  while (Serial.available()) 
  {
   char inChar = (char)Serial.read();
   Serial.print("data:");
   Serial.write(inChar);
   Serial.println();
   Serial.print(i);
  }
}*/
#include "TimerOne.h"


// tone booleans and variables
boolean TONE = LOW;
boolean prevTONE = LOW;
byte incomingBit;
boolean start = 0;
boolean timer1_flag = LOW;
boolean timer2_flag = LOW;

//data storage variables
byte newData = 0;
byte prevData = 0;
byte message[200];
byte currentByte;
unsigned int byteCounter = 0;
unsigned int bitCounter = 0;


//freq variables
unsigned int timer = 0;//counts period of wave
unsigned int timer1 = 0;//counts period of wave
unsigned int period;
int oneBitPeriod = 207;// = (16*10^6) / (1200*64) - 1 (must be <256)
int oneAndHalfBitPeriod = 312; // = (16*10^6) / (800*64) - 1
boolean toggle = 0;
boolean inLoop = 0;

// sarah's stuff
unsigned int CRC = 0xFFFF;

boolean start1 = 0;
boolean flag = 0;
boolean bitStuff = 0;

byte tone1;

int ones = 0;
int bitcount = 0;
int addresscount = 0;
int infocount = 0;
int controlcount = 0;

byte current;

byte address[14];
byte control[2];
unsigned int fcs;
byte info;

byte string[200];
int index = 0;
int bitMessage = 0;

int stopOut;
byte fcsOut;
int infoShift;




/**
 * Initialize setup
 */
void setup() {

  Serial.begin(9600);

  pinMode(12, OUTPUT); //tone indicator
  pinMode(11, OUTPUT); //tone indicator
  //digitalWrite(13, HIGH);

  //cli();//disable interrupts
  noInterrupts();
  //set up continuous sampling of analog pin 0
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;

  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only

  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements

  // timer 1 interrupt
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = oneBitPeriod;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 64 prescaler
  TCCR1B |= (1 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  //set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = oneBitPeriod;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 64 prescaler
  TCCR2B |= (1 << CS22);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  //enable interrupts
  interrupts();
}

/**
 * Interrupt for analog input
 */
ISR(ADC_vect) {//when new ADC value ready

  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (prevData < 127 && newData >= 127) { //if increasing and crossing midpoint
    period = timer;//get period from current timer value
    timer = 0;//reset timer
    prevTONE = TONE;
    // if are period is less than count for 1700 hz
    if (period > 11) {
      TONE = LOW;
      //PORTB |= B00010000;//set pin 12 high
    }
    // if are period is greater than count for 1700 hz
    else if (period <= 11) {
      TONE = HIGH;
      //PORTB &= B11101111;//set pin 12 low
    }
  }
  else if (prevData > 127 && newData <= 127) {
    period = timer;//get period from current timer value
    timer = 0;//reset timer
    prevTONE = TONE;
    // if are period is less than count for 1700 hz
    if (period > 11) {
      TONE = LOW;
      //PORTB |= B00010000;//set pin 12 high
    }
    // if are period is greater than count for 1700 hz
    else if (period <= 11) {
      TONE = HIGH;
      //PORTB &= B11101111;//set pin 12 low
    }
  }
  timer++;//increment timer at rate of 38.5kHz
}



/**
 * Interrupt for the timer 2 flag
 */
ISR(TIMER2_COMPA_vect) {
  timer2_flag = HIGH;
}


/**
 * Interrupt for the timer 1 flag
 */
ISR(TIMER1_COMPA_vect) {
  timer1_flag = HIGH;
}

/**
 * Main loop
 */
void loop() {

  if (timer2_flag)
  {
    timer2_flag = LOW;
    // start
    inLoop = 1;
    while (inLoop)
    {
      // left path
      if (prevTONE != TONE)
      {
        // toggle prev_tone
        prevTONE = !prevTONE;

        // set compare match register
        OCR1A = oneAndHalfBitPeriod;
        TCNT1  = 0;

        // clear timer flag
        timer1_flag = LOW;

        PORTB &= B11101111;//set pin 12 low
        incomingBit = 0;
        currentByte = currentByte >> 1;
        currentByte |= incomingBit; // add bit to currentbyte

        // start storing bits in message array
        if (start)
        {
          bitCounter++;
          if (bitCounter == 8) {
            bitCounter = 0;
            message[byteCounter] = currentByte;
            byteCounter++;
            if (byteCounter == 200)
            {
              // clear interrupts
              noInterrupts();
              sendPackage();
              interrupts();
            }
          }
        }// check for start byte
        else if ((currentByte == B01111110) && (start == 0))
        {
          start = 1;
          message[0] = currentByte;
          bitCounter = 0;
          byteCounter = 1;
        }
        inLoop = 0;
      }
      // right path
      else if (prevTONE == TONE && timer1_flag)
      {
        // set compare match register
        OCR1A = oneBitPeriod;
        TCNT1  = 0;

        // clear timer flag
        timer1_flag = LOW;

        PORTB |= B00010000;//set pin 12 high
        // return 1
        incomingBit = 128;
        currentByte = currentByte >> 1;
        currentByte |= incomingBit; // add bit to currentbyte

        // start storing bits in message array
        if (start)
        {
          bitCounter++;
          if (bitCounter == 8) {
            bitCounter = 0;
            message[byteCounter] = currentByte;
            byteCounter++;
            if (byteCounter == 200)
            {
              // clear interrupts
              noInterrupts();
              sendPackage();
              interrupts();
            }
          }
        }
        inLoop = 0;
      }
    }
  }

}

/**
 * This funciton is called when we have a full message buffer.
 * It sends the message via the COM line
 */
void sendPackage()
{
  //for(int i = 0;i <= byteCounter; i++)
 // {
    //print_binary(message[i], 8);
   // Serial.write(message[i]);
//  }
 // Serial.println('\n');
  printToCOM();
  byteCounter = 0;
  start = 0;
}

void print_binary(int v, int num_places)
{
  int mask = 0, n;

  for (n = 1; n <= num_places; n++)
    mask = (mask << 1) | 0x0001;
  v = v & mask;  // truncate v to specified number of places

  while (num_places)
  {
    if (v & (0x0001 << num_places - 1))
      Serial.print("1");
    else
      Serial.print("0");
    --num_places;
  }
  Serial.print("_");
}


void printToCOM()
{
  int i,j;
  
  for (i = 0; i < 200; i++)
  {
    for (j = 0; j < 8; j++)
    {
      int value  = message[i] & B00000001;
      message[i] = message[i] >> 1;

      if (value == 1)
      {
        bitStuff = 0;
        tone1 = 0x80;
        bitMessage = 1;
        if (ones == 5)
          flag = 1;
        ones++;
      }
      else if (value == 0)
      {
        bitStuff = 0;
        tone1 = 0x00;
        bitMessage = 0;
        if (ones == 5)
          bitStuff = 1;
        ones = 0;
      }
      else {
        bitStuff = 1;
      }


      if (bitStuff == 0) {
        //send
        if (current & 0x01)
        {
          stopOut = 0x8000;
        }
        else {
          stopOut = 0;
        }
        current = current >> 1;
        current |= tone1;//replace tone with the bit that is set

        if (start1 == 0)
        {
          if (current == 0x7E && flag == 1) {
            start1 = 1;
            flag = 0;
            //printf("%u", current);
            Serial.write(current);
          }
        }
        else if (start1 == 1)
        {
          bitcount++;

          if (current == 0x7E && flag == 1)
          {
            if (bitcount > 136) {
              if (fcs & 0x0001)
              {
                fcsOut = 0x80;
                infoShift = 1;
              }
              else {
                fcsOut = 0x00;
                infoShift = 0;
              }
              fcs = fcs >> 1;
              fcs |= stopOut;
            }
            if (bitcount > 152)
            {
              // send infoShift to crc function
              CRC = CRCcalc(infoShift, CRC);
              CRC = ~CRC;
              //printf("\n%u\n", CRC);
              //printf("%u\n", fcs);

              info = info >> 1;
              info |= fcsOut;
              if (bitcount % 8 == 0) {
                int infoD = info;
                string[index] = infoD;
                index++;
              }
            }

            //if fcs is equal to crc
            if (fcs == CRC) {
              for (int j = 0; j < 200; j++) {
                if (string[j] != 0)
                  Serial.write(string[j]);
              }
            }

            //printf("\n%u", current);

            for (int j = 0; j < 200; j++) {
              string[j] = 0;
            }

            start1 = 1;
            bitcount = 0;
            addresscount = 0;
            controlcount = 0;
            ones = 0;
            flag = 0;
            bitStuff = 0;
            bitMessage = 0;
            CRC = 0xffff;

            infocount = 0;
            index = 0;

            for (int i = 0; i < 14; i++) {
              address[i] = 0;
            }

            control[0] = 0;
            control[1] = 0;
            fcs = 0;
            info = 0;

            stopOut = 0;
            fcsOut = 0;
            infoShift = 0;
          }
          else if (bitcount <= 112)
          {
            //send tone to crc function
            CRC = CRCcalc(bitMessage, CRC);
            address[addresscount] = address[addresscount] >> 1;
            address[addresscount] |= tone1;
            if (bitcount % 8 == 0) {
              //printf("\n%u", address[addresscount]);
              //Serial.write(address[addresscount]);
              addresscount++;
            }
          }
          else if (bitcount <= 128) {

            //send tone to crc function
            CRC = CRCcalc(bitMessage, CRC);
            control[controlcount] = control[controlcount] >> 1;
            control[controlcount] |= tone1;
            if (bitcount % 8 == 0) {
              //printf("\n%u", control);
             // Serial.write(control[controlcount]);
              controlcount++;
            }
          }
          else {
            if (bitcount > 136) {
              if (fcs & 0x0001)
              {
                fcsOut = 0x80;
                infoShift = 1;
              }
              else {
                fcsOut = 0x00;
                infoShift = 0;
              }
              fcs = fcs >> 1;
              fcs |= stopOut;
            }
            if (bitcount > 152)
            {
              // send infoShift to crc function
              CRC = CRCcalc(infoShift, CRC);
              info = info >> 1;
              info |= fcsOut;
              if (bitcount % 8 == 0) {
                //calculate character for info byte, store in character array
                string[index] = info;
                //Serial.write(string[index]);
                index++;
              }
            }
          }
        }
      }
    }
  }
  Serial.print('\n');
}


unsigned int CRCcalc(int data, unsigned short CRC) {
    unsigned int mask = 0x0001;
    unsigned int POLY = 0x8408;
    unsigned int value;

    value = CRC & mask;
    CRC = CRC >> 1;
    
    if (value ^ data) {
      CRC = CRC ^ POLY;
    }

    return CRC;
  }




