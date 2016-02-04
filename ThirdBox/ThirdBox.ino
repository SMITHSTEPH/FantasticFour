
short LEDPins[] = {5,6,8,9,11,12,13}; //need to have 7 LEDs
const short LEDS_NUM =7;
int tempPin=0; //(analog) connected to A0
short buttonPin=2; //don't worry about this button yet
//later add wifi pins
int temperature=0;
int ASCIIconversion=48;
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

}
void intToBinary(int temperature)
{
  byte char_variable = (byte) temperature;
}
void displayTemperature()
{
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
