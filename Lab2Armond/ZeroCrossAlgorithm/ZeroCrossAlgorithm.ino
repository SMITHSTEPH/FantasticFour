
int inTone;
int prevTone;
boolean TMR1_flag;



void setup() {
  // put your setup code here, to run once:
  

}

void loop() {
  // put your main code here, to run repeatedly:
  

}

void serialEvent() {
  while (Serial.available()) {

    int inTone = (int)Serial.read();
    if(inTone == 0){
        if(prevTone == 0){
          if(TMR1_flag == true){
            //set TMR1_flag to 1 bit periods
            //clear TMR1_flag
            //return 1
          }
      }
      else{
        prevTone = 0;
        //set TMR1_flag to 1.5 bit periods
        //clear TMR1_flag
        //return 0
      }
    }
    else{
      if(prevTone == 0){
        prevTone = 1;
        //set TMR1_flag to 1.5 bit periods
        //clear TMR1_flag
        //return 0
      }
      else{
        if(TMR1_flag == true){
          //set TMR1_flag to 1 bit periods
          //clear TMR1_flag
          //return 1
        }
        else{
          //jump back to start to check if tone is equal to 0
        }
      }
    }
  }
}
