#include <avr/io.h>
#include <util/delay.h>

//configure inputs and outputs
/*global variables*/
boolean curTone=LOW;
boolean prevTone=LOW;

boolean tmr2Flag=false; //flag for timer ISR
boolean tmr1Flag=false; //flags for main loop
boolean noChange=true;
boolean change=false;

boolean toggleWave=HIGH;

//byte data[200]; //temporarily store the data
String packets=""; //store in here for now

//OCRn =  [ (clock_speed / Prescaler_value) * Desired_time_in_Seconds ] - 1
const unsigned short bitPeriod=207; //floor([(16 MHz/64) * 1/1200 Hz] -1)
                                    //use 16-bit timer b/c bitPeriod*1.5>255 (which is the max for the 8-bit timer)
const unsigned short threshold=72;  //floor([(16 MHz/128) * (1/1700)Hz] - 1)

const unsigned short signal2200=112/2; //floor([(16MHz/64) * 1/2200 Hz]-1) --> will have a frequency of 2200 and a 50% duty cycle
const unsigned short signal1200=207/2; //floor([(16MHz/64) * 1/1200 Hz]-1) --> will have a frequency of 1200 and a 50% duty cycle

/**
 * configuring I/O ports
 * configuring timers
 * configuring interrupts
 */
void setup()
{
  Serial.begin(19200); //for printing to serial console
  DDRD = 1<<PD0; //configure PD0 (tx pin) as output leave the rest as inputs
  DDRB = 1<<PB2; //make digitial pin 13 and output
  //DD
  
  EICRA |= (1 << ISC10); //trigger on any change
  EIMSK |= (1 << INT0); // turns on INT0  

  //configure 16 bit timer1 (used in main loop)
  TCCR1B |= (1 << WGM12);   //set the Timer Mode (mode 4) to CTC (Clear Timer on Compare)
  TIMSK1 |= (1 << OCIE1A);  //set interrupt on compare match

  //configure 8 bit timer2 (used in tone ISR)
  OCR2A = threshold;        //always count up to the threshold
  TCCR2A |= (1 << WGM21);   //set to CTC Mode
  TIMSK2 |= (1 << OCIE2A);  //set interrupt on compare match

  //configure 8 bit timer0 (used to generate square wave for testing)
  TCCR0A |= (1 << WGM01);   //set the Timer Mode to CTC
  //OCR0A = signal2200;
  OCR0A = signal1200;
  TIMSK0 |= (1 << OCIE0A);  //set interrupt on compare match
  TCCR0B |= (1 << CS01) | (1 << CS00); //set prescaler to 64 and start the timer
  
  PORTB |= (1<<PB2); //set PB5 (digital pin 13) high

  //_delay_ms(2000);

  
  sei(); //enable all global interrupts 
  
}
/**
 * NRZI algorithm
 */
void loop()
{
   noChange=(curTone==prevTone);
   change=(curTone!=prevTone);

   //packets+=curTone;
   
   if((noChange && tmr1Flag))
   {
        stopTimer1();
        startTimer1(bitPeriod); //set TMR1 to 1 bit periods
        tmr1Flag=false;
        packets+="1";
   }
   else if(change)
   {
      stopTimer1();
      prevTone=LOW; //toggle prevTone
      startTimer1(bitPeriod*1.5); //set TMR1 to 1.5 bit periods
      tmr1Flag=false;
      packets+="0";
   }
}

/**utility timer functions**/
void startTimer2()
{
  TCCR2B |= (1 << CS22) | (1<<CS20);
}
void stopTimer2()
{
  TCCR2B &= ~(1<<CS22);
  TCCR2B &= ~(1<<CS20);
}
void stopTimer1()
{
  TCCR1B &= ~(1 << CS10); 
  TCCR1B &= ~(1 << CS11);
}
void startTimer1(const unsigned short count)
{
  OCR1A = count; //count to one bit period
  TCCR1B |= (1<<CS11) |(1 << CS10); 
  //set prescaler to 64 and start timer
}
/**
 * Interrupt configured for digital pin 2
 * It will trigger once every period (on the rising edge of the pulse)
 */
ISR(INT0_vect)
{
  prevTone=curTone;
  if(tmr2Flag){
    curTone=LOW;
    //packets+="0";
    } //if period is greater than 1/1700
  else{
    curTone=HIGH;
    //packets+="1";
  } //otherwise it's a one
  tmr2Flag=false; //restart timer
  stopTimer2();
  startTimer2();
}
/**
 * interrupt triggered every 1 or 1.5 bit period(s)
 */
ISR(TIMER1_COMPA_vect){tmr1Flag=true;}
/*
 * interrupt trigger every 1/1700 seconds
 */
ISR (TIMER2_COMPA_vect){tmr2Flag=true;}
ISR(TIMER0_COMPA_vect)
{
  toggleWave^=1; //xor toggleWave
  //Serial.println(toggleWave);
  
  PORTB= toggleWave ? PORTB & ~(1<<PB2) : PORTB | (1<<PB2); //if wave is high clear bit else set bit

  TCCR0B &= ~(1 << CS01); //stop timer
  TCCR0B &= ~(1 << CS00); 
  
  TCCR0B |= (1 << CS01) | (1 << CS00); //start timer
 
}
/**used for testing nzri algorithm**/
void serialEvent()
{
  while (Serial.available()) 
  {
   char inChar = (char)Serial.read();
   Serial.print("data:");
   Serial.write(inChar);
   Serial.println(packets);
   
  }
}
