#include <EnableInterrupt.h>

#define SERIAL_PORT_SPEED 57600
#define RC_NUM_CHANNELS  4

#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3

#define RC_CH2_INPUT  A1
#define RC_CH3_INPUT  A2
#define RC_CH4_INPUT  A3


uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

#define LM1 2
#define LM2 3
#define RM1 4
#define RM2 5
#define R 10
#define G 11
#define UV1 9
#define UV2 8

//int ch2 = A1;
//int ch3 = A2;
//int ch4 = A3;

int axis1, axis2,axis3, axis4;

void rc_read_values() {
  noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
  interrupts();
}


void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
  }
}

void calc_ch2() { calc_input(RC_CH2, RC_CH2_INPUT); }
void calc_ch3() { calc_input(RC_CH3, RC_CH3_INPUT); }
void calc_ch4() { calc_input(RC_CH4, RC_CH4_INPUT); }
//////////////////////////////////////////////
int checkCh2(){
    rc_read_values();
    return ((int)rc_values[RC_CH2]);
}
int starttime;

int checkStart(){
    int minth=2400;
    int maxth=1200;
    int centh=1800;
    int t1 = millis();
    
    // wait untill max 
    while(checkCh2()<maxth){delay(100);}    
    // check if max for 2 sec 
    t1 = millis();
    while ((millis() - starttime) <=2000){
        if(checkCh2()<maxth){return 0;}
    }
    
    // wait untill min 
    while(checkCh2()>minth){delay(100);}    
    // check if max for 2 sec 
    t1 = millis();
    while ((millis() - starttime) <=2000){
        if(checkCh2()>minth){return 0;}
    }
    
    // wait untill center 
    while(checkCh2()>(centh-.1*centh)  and checkCh2()<(centh+.1*centh)){delay(100);}    
    
    return 1;
    
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(RC_CH2_INPUT, INPUT);
    pinMode(RC_CH3_INPUT, INPUT);
    pinMode(RC_CH4_INPUT, INPUT);

    enableInterrupt(RC_CH2_INPUT, calc_ch2, CHANGE);
    enableInterrupt(RC_CH3_INPUT, calc_ch3, CHANGE);
    enableInterrupt(RC_CH4_INPUT, calc_ch4, CHANGE);

//    pinMode(ch2, INPUT);
//    pinMode(ch3, INPUT);
//    pinMode(ch4, INPUT);
    pinMode(LM1, OUTPUT);
    pinMode(LM2,OUTPUT);
    pinMode(RM1, OUTPUT);
    pinMode(RM2, OUTPUT);
    pinMode(G, OUTPUT);
    pinMode(R, OUTPUT);
    pinMode(UV1, OUTPUT);
    pinMode(UV2, OUTPUT);

    digitalWrite(G,LOW);
    digitalWrite(R,LOW);
    while(checkStart()==0){}
    
}

void loop() {
  // put your main code here, to run repeatedly:
  rc_read_values();

  Serial.print("CH2:"); Serial.print(rc_values[RC_CH2]); Serial.print("\t");
  Serial.print("CH3:"); Serial.print(rc_values[RC_CH3]); Serial.print("\t");
  Serial.print("CH4:"); Serial.println(rc_values[RC_CH4]); Serial.print("\t");
  delay(200);
  
  axis2 = rc_values[RC_CH2];
  axis4 = rc_values[RC_CH4];
  
if(axis2 > 1480 && axis2 < 1510 && axis4 > 1460 && axis4 < 1480)
{
  endMovement();
}
if(axis2 > 1800 ){ 
  moveForward();
}
if(axis2 < 1253){ 
  moveBackward();
  }
if(axis4 < 1115 ){ 
  moveRight();
  }
if(axis4 > 1823 ){ 
  moveLeft();
}

if(axis2 == 0 || axis4 == 0){
  endMovement();
  }
if(axis2 >2000 || axis4 > 2000){
  endMovement();
  }
if ( rc_values[RC_CH3] < 1000){
  digitalWrite(UV1,HIGH);
  digitalWrite(UV2,HIGH);
  digitalWrite(R,HIGH);
  Serial.println("UV ON");
  delay(100);
  
}
else{
  digitalWrite(UV1,LOW);
  digitalWrite(UV2,LOW);
  digitalWrite(G,HIGH);
  Serial.println("UV Off");
  delay(100);
  
}
}

void moveForward(){
  digitalWrite(G,LOW);
  digitalWrite(R,HIGH);
  digitalWrite(LM1, HIGH);
  digitalWrite(LM2, LOW);
  digitalWrite(RM1, HIGH);
  digitalWrite(RM2, LOW);
  Serial.println("forward");
  delay(100);
  }


void endMovement(){
  digitalWrite(G,HIGH);
  digitalWrite(R,LOW);
  digitalWrite(LM1, LOW);
  digitalWrite(LM2, LOW);
  digitalWrite(RM1, LOW);
  digitalWrite(RM2, LOW);
  Serial.println("stop");
  delay(100);
}

void moveBackward(){
  digitalWrite(G,LOW);
  digitalWrite(R,HIGH);
  digitalWrite(LM1, LOW);
  digitalWrite(LM2, HIGH);
  digitalWrite(RM1, LOW);
  digitalWrite(RM2, HIGH);
  Serial.println("back");
  delay(100);
}

void moveLeft(){
  digitalWrite(G,LOW);
  digitalWrite(R,HIGH);
  digitalWrite(LM1, HIGH);
  digitalWrite(LM2, LOW);
  digitalWrite(RM1, LOW);
  digitalWrite(RM2, LOW);
  Serial.println("left");
  delay(100);
}

void moveRight(){
  digitalWrite(G,LOW);
  digitalWrite(R,HIGH);
  digitalWrite(LM1, LOW);
  digitalWrite(LM2, LOW);
  digitalWrite(RM1, HIGH);
  digitalWrite(RM2, LOW);
  Serial.println("right");
  delay(100);
}
