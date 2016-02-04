
short LEDPins[] = {5,6,8,9,11,12,13}; //need to have 7 LEDs
const short LEDS_NUM =7;
int tempPin=0; //(analog) connected to A0
short buttonPin=2; //don't worry about this button yet
//later add wifi pins
int temperature=0;
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
  displayTempTest(temperature);
  byte test=intToByte(temperature);
  displayTemperature(test);

}
/**
 * This function converts the integer temp to a byte
 */
byte intToByte(int temperature)
{
  if(temperature <= 63 || temperature >= -64) //possible range of values the the 7 LEDs can display
  {
    byte char_variable = (byte) temperature;
    return char_variable;
  }
}
void displayTemperature(byte bTemp)
{
  int startingIndex=0;
  String wholeByte=String(bTemp, BIN);
  if(wholeByte.length() == 8) //will only have a length of 8 when # is negative 
  { 
    startingIndex=1; //take off the MSB
  }
  else
  {
    for(int i=0; i<(7-wholeByte.length()); i++)
    {
      Serial.print("0");
    }
  }
  for(int i = startingIndex; i<wholeByte.length(); i++)
  {
    Serial.print(wholeByte.charAt(i));
  }
  Serial.println();
  
  
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
  delay(500); //delays 1/2 a second
}
/**
 * This function prompts a user to enter temperature values.
 */
int generateTempTest()
{
  Serial.print("enter temp in degrees C: ");
  while (Serial.available() <= 0){} //get the number of bytes available for reading from the serial port
  int incomingTemp = Serial.readString().toInt(); // read the incoming string from the user and converts it an integer
  Serial.println(); 
  return incomingTemp;
  
}
