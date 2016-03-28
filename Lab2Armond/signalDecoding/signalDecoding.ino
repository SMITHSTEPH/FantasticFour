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
String flagSearch = "";
boolean readingPacket= false;
String buildPacket="01111110";

String packets[1200];
int packetsCount=0;

void setup()
{
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    //pinMode(pin, INPUT);
    /*
    String flag = "01111110";
    String address = "1110000011100000111000001110000011100000111000001110000011100000111000001110000011100000111000001110000011100000";
    String control = "0000111101001111";
    String info = "0100100001000101010011000100110001001111001000000101011101001111010100100100110001000100";
    String checksum = "0110011001100110";
    */
    String binary = flag + address + control + info + checksum + flag;
    sendDecodedPacketToPC(binary);
    computeChecksum(flag);
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

  /*
    String currentBit;
    if(readingPacket == true){
        buildPacket.concat(currentBit);
    }
    
    int flagSearchLength = flagSearch.length();
    if(flagSearchLength == 8){
        flagSearch = flagSearch.substring(1, 8);
        flagSearch.concat(currentBit);
        if(flagSearch.equals("01111110")){
            if(readingPacket == false){
                readingPacket= true;
            }else{
                readingPacket= false;
                packets[packetsCount]= buildPacket;
                sendDecodedPacketToPC(buildPacket);
                //RESET PACKET
                buildPacket= "01111110";
            }
            
        }
    }
    */
  
}

/***************************************************************
 *    PARSES A BINARY PACKET TO BREAK BYTES INTO
 *    START FLAG, ADRESS, CONTROL, INFORMATION,
 *    CRC, AND STOP FLAG
 ***************************************************************/
void sendDecodedPacketToPC(String packetBinaryString){
    String packetFlag = "01111110"; //start and stop flags
    int beginningFlagStart = packetBinaryString.indexOf(packetFlag);
    int closingFlagStart = packetBinaryString.indexOf(packetFlag, beginningFlagStart + 1);
    int beginningFlagStop = beginningFlagStart + 8;
    int closingFlagStop = closingFlagStart + 8;
    
    int addressStartPosition = beginningFlagStop;
    int addressStopPosition = addressStartPosition + 112;
    
    int controlStartPosition = addressStopPosition;
    int controlStopPosition = controlStartPosition + 16;
    
    int checkSumStopPosition= closingFlagStart;
    int checkSumStartPosition= checkSumStopPosition-16;
    
    int infoStartPosition = controlStopPosition;
    int infoStopPosition = checkSumStartPosition;

    String startFlag= packetBinaryString.substring(beginningFlagStart, beginningFlagStop);
    String address= packetBinaryString.substring(addressStartPosition, addressStopPosition);
    String control= packetBinaryString.substring(controlStartPosition, controlStopPosition);
    String info= packetBinaryString.substring(infoStartPosition, infoStopPosition);
    String checksum= packetBinaryString.substring(checkSumStartPosition, checkSumStopPosition);
    String checksumCorrect = stringReverse(checksum);
    String stopFlag= packetBinaryString.substring(closingFlagStart, closingFlagStop);

    int infoLength= info.length();
    //H = 72; E=69 ; L =76 ; L =76 ; O= ; space= 32; W=87 O=79 R=82 L=76 D=68

    Serial.println("Start Flag: " + startFlag);
    Serial.println("Address: " + address);
    Serial.println("Control: " + control);
    Serial.println("Checksum: " + checksum);
    Serial.println("Stop Flag: " + stopFlag);
    Serial.println("");
    Serial.print("DECODED MESSAGE: ");
    for(int i=0; i< infoLength; i++){
        int temp = i+1;
        if (temp % 8 == 0){
            String tempByte = info.substring(temp-8, temp);
            int decimal= binaryToDecimal(tempByte);
            Serial.write(decimal);
        }
    }
    Serial.println("");
}


/***************************************************************
 *    REVERSES A STRING
 ***************************************************************/
String stringReverse(String stringToReverse){
    int len =  stringToReverse.length();
    String newString;
    char tempChar;
    for(int i=len-1; i>= 0; i--){
        tempChar= stringToReverse.charAt(i);
        newString.concat(tempChar);
    }
    return newString;
}


/***************************************************************
 *    CONVERT A BINARY STRING TO A DECIMAL
 ***************************************************************/
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

String computeChecksum(String DATA){
    //data is a binary string
    //must be converted into hex
    //hex values can then perform logic
    String newCRC;
    String CRC= "1111111111111111";
    String POLY= "1000010000001000";
    int dataLength = DATA.length();
    int currentPlace= dataLength-1;
    char dataChar= DATA.charAt(currentPlace);
    String dataCharToString= String(dataChar);
    CRC.substring(1);
    CRC= dataCharToString+CRC;

    for(int j=0; j<dataLength; j++){
        dataChar= DATA.charAt(currentPlace);
        dataCharToString= String(dataChar);
        CRC.substring(0, 14); //shaves least significant bit off string
        CRC= dataCharToString+CRC;
        Serial.println("CRC: " + CRC);
        for(int i=0; i<16; i++){
            if(CRC.charAt(i) == (POLY.charAt(i))){
                  newCRC.concat("0");
            }
            else{
                newCRC.concat("1");
            }
        }
        CRC= newCRC;
        Serial.println("CRC: " + CRC);
        newCRC="";
        currentPlace--;
    }


    Serial.println("FINAL CRC: " + CRC);
    return CRC;
}










