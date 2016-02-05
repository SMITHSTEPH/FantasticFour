
const short LEDPins[] = {5,6,8,9,11,12,13}; //need to have 7 LEDs
const short LEDS_NUM =7;
const short buttonPin=2; //don't worry about this button yet
const int tempPin=0; //(analog) connected to A0
int temperature=0;

//later add wifi pins
void setup() 
{
  for(int i=0; i<LEDS_NUM; i++) //configure LEDs
  {
    pinMode(LEDPins[i], OUTPUT);
  }
  pinMode(A0, INPUT); //configure temp sensor input
  pinMode (buttonPin, INPUT);
  
  Serial.begin(9600); //FOR TESTING
}
void loop() 
{
  //temperature=readTempSensor();
  int temperature= generateTempTest();
  fastTempDisplay(temperature);
  //displayTempTest(temperature);
  //String test=intToBitString(temperature);
  //Serial.println(test);
}
/**
 * This function converts the integer temperature into a bit String. 
 * -- if the third box is too slow revise this function 
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
/**
 * This function reads in the analog input and converts this input
 * to a temperature (in degrees celsius) by using this ADC formula:
 * temperature = (reference voltage/resolution) * (analog voltage measured) * 100
 * temperature = (analog voltage measured) * 100 * (5.00 V/1024)
 * temperature = (analog voltage measured) * 0.48828125
 */
int readTempSensor()
{
  int temperature = analogRead(tempPin); //casting input to integer
  temperature = temperature * 0.48828125;
  return temperature;
}
/**
 * This function displays the temperature (in degrees C) to the serial monitor. 
 * This function is for TESTING PURPOSES ONLY
 */
void displayTempTest(int temperature)
{
  Serial.print("Integer Temperature = "); //prints out the temperature to the serial prompt
  Serial.print(temperature);
  Serial.println("*C");
  //delay(500);
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
  * This functin lights up the LED display quickly
  */
 void fastTempDisplay(int temp)
 {
    int j=6;
    for(int i=0; i<LEDS_NUM; i++)
    {
      digitalWrite(LEDPins[j], (0x01<<i)&((byte)temp)); //better way to do this
      j--;
    }

    /*digitalWrite(LEDPins[6], 0x01&((byte)temp)); //hard coded values
    digitalWrite(LEDPins[5], 0x02&((byte)temp));
    digitalWrite(LEDPins[4], 0x04&((byte)temp));
    digitalWrite(LEDPins[3], 0x08&((byte)temp));
    digitalWrite(LEDPins[2], 0x10&((byte)temp));
    digitalWrite(LEDPins[1], 0x20&((byte)temp));
    digitalWrite(LEDPins[0], 0x40&((byte)temp));*/
    //digitalWrite(LEDPins[6], 0x00 & ((byte)temp)); //TEST --> turning off  built in LED
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
 void writeAllLEDS(int mode)
 {
    for(int i = 0; i < LEDS_NUM; i++)
    {
      digitalWrite(LEDPins[i], mode);
    }
 }
