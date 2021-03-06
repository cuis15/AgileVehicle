#include <FlexiTimer2.h>
#define PUL 51				//3 for nano
#define DIR 53				//4 for nano
#define csn 31 // yellow	//7 for nano
#define dat 33 // green		//6 for nano
#define clk 35 // blue		//8 for nano
int encoder_resolution = 4096;
String inputString = "";
String inputAngle  = "";
int pulseTime = 100;
int angleTime = 100;
int angle = 0;
int desiredAngle = 0;
bool angleComplete = false;

typedef struct {
  char end_1;
  char end_2;
  unsigned short x;
  unsigned short y;
} serial_format;

void setup() {
  Serial.begin(9600);
  inputString.reserve(100);
  inputAngle.reserve(100);
  // set driving control
  pinMode(DIR, OUTPUT);
  pinMode(PUL, OUTPUT);
  // set angle encoder
  pinMode(csn, OUTPUT);
  pinMode(dat, INPUT);
  pinMode(clk, OUTPUT);
  encoder();
  FlexiTimer2::set(angleTime, encoder);
  FlexiTimer2::start();
}
int data = 0;
int counter = 0;
void loop() {
  if(angleComplete)
    {
        //Serial.println(inputAngle);
        desiredAngle = inputAngle.toInt();
        //Serial.println(desiredAngle);
        angleComplete = false;
        
        inputAngle="";
   }
    // As the first version has only one encoder (for the angle), only the angle part has the close loop control.
    //-------------------start angle control------------------------------------
    if(desiredAngle < 0 || desiredAngle >encoder_resolution) { // will be modified as -90 degree to 90 degree
        //Serial.println("bad DesiredAngle input.");
    }
    else {
        if(!(abs(desiredAngle-angle)<40 ||abs(desiredAngle-angle+encoder_resolution)<40||abs(desiredAngle-angle-encoder_resolution)<40)) {
            if (desiredAngle>angle) {
                OneUp(pulseTime,1);
            }
            else {
                OneUp(pulseTime,2);
            }  
        }
        //end while loop 
    }
    //----------------end angle control---------------------------------------------  
    
}

void serialEvent(){
    while(Serial.available()){
        char inChar = (char)Serial.read();
        if(inChar != 's' && inChar != 'd' && inChar != 'b'){
            inputString+=inChar;
        }
        if(inChar == 's'){ // s是转角数据开头
            angleComplete = true;
            inputAngle=inputString;
            inputString="";
            return;
        }
        if(inChar == 'd'){ // d是驱动数据开头
            inputString="";
            return;
        }
    }
    //END WHILE LOOP
}

// the function to move the angle motor for once
void OneUp(unsigned int time, unsigned int direc) {
  if (direc == 1){
    digitalWrite(DIR,HIGH);
  }
  else if (direc == 2) {
    digitalWrite(DIR,LOW);
  }
  // give a pulse
  digitalWrite(PUL,HIGH);
  delayMicroseconds(time);
  digitalWrite(PUL,LOW);
  delayMicroseconds(time);
   //unit of time is us
   delay(2);
}

void encoder()
{
  int val = 0;
  data=0;
  digitalWrite(csn,LOW);
  delayMicroseconds(1);
  for (int k=0;k<12;k++)
  {
    digitalWrite(clk,LOW);
    delayMicroseconds(1);
    digitalWrite(clk,HIGH);
    delayMicroseconds(1);
    val=digitalRead(dat);
    data=(data<<1)+val;
    //delayMicroseconds(1);
  }
  for (int k=0;k<6;k++)
  {
    digitalWrite(clk,LOW);
    delayMicroseconds(1);
    digitalWrite(clk,HIGH);
    delayMicroseconds(1);
  }
  digitalWrite(csn,HIGH);
  angle = data;
  //Serial.println(angle);
  serial_format to_send;
  to_send.end_1=0x3f;
  to_send.end_2=0x3f;
  to_send.x=angle;
  to_send.y=0;
  Serial.println(to_send.x);
  Serial.println(desiredAngle);
  //Serial.println(sizeof(serial_format));
  Serial.write((const uint8_t*)&to_send,sizeof(serial_format));
}
