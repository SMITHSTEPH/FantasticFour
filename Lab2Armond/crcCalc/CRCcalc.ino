void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  int POLY = 0x8408;
  int MASK = 0x0001;

  int crc = 0xFFFF;
  int data = 0x54;

  String hexDataString = String(data, HEX);
  int stop = (hexDataString.length())/2;
  String stopString = String(stop, HEX);
    
    Serial.print(data);
    Serial.print(hexDataString);
    Serial.print(stop);
    Serial.print(stopString);
    
    for(int i = 0; i < stop*8; i++)
    {
      int databit = data & MASK;
      int crcbit = data & MASK;
      crc>>=1;
      if((databit ^ crcbit) == 1)
      {
        crc ^= POLY;
      }
      
      data >>= 1;
    }
    
    crc ^= 0xFFFF;
    String crcString = String(crc, HEX);
    
    Serial.print("CRC = " + crc);
    Serial.print("CRC = " + crcString);
}
