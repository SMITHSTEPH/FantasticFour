/*********************************************************
 *        READING PULSES WITH ARDUINO PULSE IN LIBRARY
 *        Threshold = 1700 Hertz
 *        1700 Hertz = 0.5882 millisecond wave
 *        0.5882 milliseconds = 588 microseconds
 *********************************************************/
int packet[1600];
int packetCount= 0;
int bitVal= 0;
String packetString="";
int pin = 7;
unsigned long duration;

void setup()
{
    pinMode(pin, INPUT);
}

void loop()
{
    duration = pulseIn(pin, HIGH); //the signal will be high regardless
    if(duration == 0){
        //FINISHED READING SIGNAL
        //SEND 'packet' to PC via Serial Communication
        //sendDecodedPacketToPC(packet, packetString);
    }
    else{
        if(duration >588){
        bitVal = 0;
    }
    else{
        bitVal= 1;
    }
    packetString.concat(bitVal);
    packet[packetCount] = bitVal;
    packetCount++;
  } 
}

void sendDecodedPacketToPC(int decodedPacket[], String packetBinaryString){
    String packetFlag = "01111110";
    int beginningFlagStart = packetBinaryString.indexOf(packetFlag);
    int beginningFlagStop =
    int closingFlag = packetBinaryString.indexOf(packetFlag, beginningFlag + 1);
    int addressStartPosition = beginningFlag + 1;
    int addressStopPosition = addressStartPosition + 112;
    int controlStartPosition = addressStopPosition + 1;
    int InfoPosition;
    int checkSumPositionStop= closingFlag;
    int checkSumPositionStart= checkSumPositionStop-16;
    
    
    
    //String flag
}
}

