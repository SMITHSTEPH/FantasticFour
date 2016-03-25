/*********************************************************
 *        READING PULSES WITH ARDUINO PULSE IN LIBRARY
 *        Threshold = 1700 Hertz
 *        1700 Hertz = 0.5882 millisecond wave
 *        0.5882 milliseconds = 588 microseconds
 *********************************************************/
//int packet[1600];
//int packetCount= 0;
//int bitVal= 0;
//String packetString="";
//int pin = 7;
//unsigned long duration;

void setup()
{
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    
    Serial.println("HELLO");
    //pinMode(pin, INPUT);
    String flag = "01111110";
    String address = "1110000011100000111000001110000011100000111000001110000011100000111000001110000011100000111000001110000011100000";
    String control = "00001111";
    String info = "0100100001000101010011000100110001001111001000000101011101001111010100100100110001000100";
    String checksum = "0110011001100110";
    
    String binary = flag + address + control + info + checksum + flag;
    sendDecodedPacketToPC(binary);  
}



void loop()
{
  
  /*
    duration = pulseIn(pin, HIGH);
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
  */
}

void sendDecodedPacketToPC(String packetBinaryString){
    String packetFlag = "01111110";
    int beginningFlagStart = packetBinaryString.indexOf(packetFlag);
    int closingFlagStart = packetBinaryString.indexOf(packetFlag, beginningFlagStart + 1);
    int beginningFlagStop = beginningFlagStart + 8;
    int closingFlagStop = closingFlagStart + 8;
    
    int addressStartPosition = beginningFlagStop;
    int addressStopPosition = addressStartPosition + 112;
    
    int controlStartPosition = addressStopPosition;
    int controlStopPosition = controlStartPosition + 8;
    
    int checkSumStopPosition= closingFlagStart;
    int checkSumStartPosition= checkSumStopPosition-16;
    
    int infoStartPosition = controlStopPosition;
    int infoStopPosition = checkSumStartPosition;

    String startFlag= packetBinaryString.substring(beginningFlagStart, beginningFlagStop);
    String address= packetBinaryString.substring(addressStartPosition, addressStopPosition);
    String control= packetBinaryString.substring(controlStartPosition, controlStopPosition);
    String info= packetBinaryString.substring(infoStartPosition, infoStopPosition);
    String checksum= packetBinaryString.substring(checkSumStartPosition, checkSumStopPosition);
    String stopFlag= packetBinaryString.substring(closingFlagStart, closingFlagStop);

    int decodedMessage[200];
    int count=0;
    int infoLength= info.length();
    for(int i=0; i< infoLength; i++){
        int temp = i+1;
        
        if (temp % 8 == 0){
            String tempByte = info.substring(temp-8, temp);
            int decimal= binaryToDecimal(tempByte);
            Serial.write(decimal);
        }
    }
    Serial.println("");
    //H = 72; E=69 ; L =76 ; L =76 ; O= ; space= 32; W=87 O=79 R=82 L=76 D=68

    Serial.println("Start Flag: " + startFlag);
    Serial.println("Address: " + address);
    Serial.println("Control: " + control);
    Serial.println("MESSAGE: " + info);
    Serial.println("Checksum: " + checksum);
    Serial.println("Stop Flag: " + stopFlag);
    Serial.println("");
}

int binaryToDecimal(String currentByte){
    int decimalVal=0;
    
    if(currentByte.charAt(0) == '1'){
        decimalVal+= 128;
    }
    if(currentByte.charAt(1) == '1'){
        decimalVal+= 64;
    }
    if(currentByte.charAt(2) == '1'){
        decimalVal+= 32;
    }
    if(currentByte.charAt(3) == '1'){
        decimalVal+= 16;
    }
    if(currentByte.charAt(4) == '1'){
        decimalVal+= 8;
    }
    if(currentByte.charAt(5) == '1'){
        decimalVal+= 4;
    }
    if(currentByte.charAt(6) == '1'){
      decimalVal+= 2;
    }
    if(currentByte.charAt(7) == '1'){
        decimalVal+= 1;
    }
    return decimalVal;
}

