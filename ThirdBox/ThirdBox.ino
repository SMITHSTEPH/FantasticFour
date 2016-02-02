
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
  Serial.begin(9600); //for testing
}

void loop() 
{
  temperature=readTempSensor();
  //TEST!
  Serial.print("TEMPRATURE = ");
  Serial.print(temperature);
  Serial.print("*C");
  Serial.println();
  delay(1000);

}
void intToBinary(int temperature)
{
}
void displayTemperature()
{
}
int readTempSensor()
{
  int temperature = analogRead(tempPin);
  temperature = temperature * 0.48828125;
  return temperature;
}
