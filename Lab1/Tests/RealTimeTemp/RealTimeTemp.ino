bool pcReady=false;
void setup() {Serial.begin(9600); }//for serial communication with PC
void loop() 
{
  if(pcReady){Serial.println("25");}
  delay(100);
}
void serialEvent()
{
  short pcData = Serial.readString().toInt();
  //Serial.println("interrupt data is: " + String(pcData, DEC)); //for testing
  if(pcData==0){pcReady=true;}
  else if(pcData==1){pcReady=false;}
}
