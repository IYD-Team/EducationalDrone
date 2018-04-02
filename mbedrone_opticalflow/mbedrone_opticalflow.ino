#include "def.h"
#include "Bitcraze_PMW3901.h"
#include <Wire.h>
#include <VL53L0X.h>
#include <SoftwareSerial.h>
void rcUpdate(int ch[]);
void rcInit();
void serialInit();
void sendSerial(int ch[],int);
void poscontrol(int ch[],int outV[]);
void mbedSerialInit();
void sendMbedSerial(char,int senddata[],uint8_t);
//int ultraSonicReadCenti();
//void ultraSonicInit();
void idou(int,int,int);
//void chuugaeri(int);
char arm_status=DISARM;
void rc_csc_check(int ch[]);

SoftwareSerial mbedSerial(MBED_RX,MBED_TX);
Bitcraze_PMW3901 flow(10);
VL53L0X rangeSensor;

float rangeV=0.0;
float rangeVraw[FILTERSIZE];
void setup() {
  //おまじない
  delay(5000);
  rcInit();
  mbedSerialInit();
  
  #ifdef DEBUG_RC_SERIAL
    serialInit();
  #endif
  
  //initialize opticalflow sensor
  if (!flow.begin()) {
    #ifdef DEBUG_RC_SERIAL
    Serial.println("Initialization of the flow sensor failed");
    #endif
    while(1) { }
  }

   // Initialize range sensor
  Wire.begin();
  rangeSensor.init();
  rangeSensor.setTimeout(500);

  //experimental//
  //ultraSonicInit();
  for(uint8_t i=0;i<FILTERSIZE;i++){
    rangeVraw[i]=rangeSensor.readRangeSingleMillimeters();
  }
}
int16_t deltaX,deltaY;
int mode_sw=0;

void loop() {
  int ch[CH_NUM];
  int cm=0;
 int outV[CH_NUM];
  //プロポの状態を更新
  rcUpdate(ch);

  //プロポのcsc(離着陸コマンド)の認識
  rc_csc_check(ch);

  //オプティカルフローセンサの値を更新
  flow.readMotionCount(&deltaX, &deltaY);//horizontal position
  //レンジセンサ(高度)の更新
  
  rangeV=rangeVraw[0];
  for(uint8_t i=0;i<FILTERSIZE-1;i++){
    rangeVraw[i]=rangeVraw[i+1];
    if(rangeVraw[i]<rangeV){
      rangeV=rangeVraw[i];
    }
  }
  rangeVraw[FILTERSIZE-1] = rangeSensor.readRangeSingleMillimeters();
    if(rangeVraw[FILTERSIZE-1]<rangeV){
      rangeV=rangeVraw[FILTERSIZE-1];
    }
  poscontrol(ch,outV);
  //離陸可能状態であるかのチェック
  if(arm_status==ARM){
    //Serial.print("ARM!!!");
    if(mode_sw){
      idou(outV[RIGHT_VERTICAL],outV[RIGHT_HORIZONTAL],outV[LEFT_VERTICAL],outV[LEFT_HORIZONTAL]);
    }else{
      idou(ch[RIGHT_VERTICAL],ch[RIGHT_HORIZONTAL],ch[LEFT_VERTICAL],ch[LEFT_HORIZONTAL]);
    }
  }else{
    disarm();
    //Serial.print("DISARM!!!");
  }
  delayMicroseconds(2000);

  
  //for debugging
  #ifdef DEBUG_RC_SERIAL
    if(mode_sw){
         sendSerial(outV,cm);
    }else{
         sendSerial(ch,cm);
    }
 
  #endif
  //asobi
  //cm=ultraSonicReadCenti();

  //chuugaeri(3);

}

void rcUpdate(int ch[CH_NUM]){
  ch[RIGHT_HORIZONTAL] = pulseIn(rcPin[RIGHT_HORIZONTAL], HIGH, 25000); // Read the pulse width of 
  ch[RIGHT_VERTICAL] = pulseIn(rcPin[RIGHT_VERTICAL], HIGH, 25000); // each channel
  ch[LEFT_HORIZONTAL] = pulseIn(rcPin[LEFT_HORIZONTAL], HIGH, 25000);
  ch[LEFT_VERTICAL] = pulseIn(rcPin[LEFT_VERTICAL], HIGH, 25000);
  mode_sw = pulseIn(RC6CHPIN, HIGH, 25000);
  if(mode_sw>1500){
    mode_sw=1;
  }else{
    mode_sw=0;
  }
  for(uint8_t stick_num_count=0;stick_num_count<4;stick_num_count++){
    if(ch[stick_num_count]>1900){
      ch[stick_num_count]=1900;
    }else if(ch[stick_num_count]<1100){
      ch[stick_num_count]=1100;
    }
    ch[stick_num_count]=(ch[stick_num_count]-1500)/4;
    if((ch[stick_num_count]>-6)&&(ch[stick_num_count]<6)){
      ch[stick_num_count]=0;
    }else if (ch[stick_num_count]>=6){
       ch[stick_num_count]-=6;
    }else if (ch[stick_num_count]<=-6){
       ch[stick_num_count]+=6;
    }
    ch[stick_num_count]=ch[stick_num_count]*100/94;
  }
}

void rcInit(){
  pinMode(rcPin[RIGHT_HORIZONTAL], INPUT); // Set our input pins as such
  pinMode(rcPin[LEFT_VERTICAL], INPUT);
  pinMode(rcPin[RIGHT_HORIZONTAL], INPUT);
  pinMode(rcPin[LEFT_VERTICAL], INPUT);
  pinMode(RC6CHPIN, INPUT);
}

void serialInit(){
  Serial.begin(500000); // Pour a bowl of Serial
   Serial.print("Serial just began.\n");
}

void sendSerial(int ch[CH_NUM],int cm){
 
    Serial.print(ch[RIGHT_VERTICAL]);
  Serial.print("\t"); // Print the value of 
    Serial.print(ch[RIGHT_HORIZONTAL]);
  Serial.print("\t"); // Print the value of 
    Serial.print(ch[LEFT_VERTICAL]);
  Serial.print("\t"); // Print the value of 
    Serial.print(ch[LEFT_HORIZONTAL]);
  Serial.print("\t"); // Print the value of 
    Serial.print(mode_sw);
  Serial.print("\t"); // Print the value of   
  
  Serial.print("X: ");
    Serial.print(deltaX);
  Serial.print("\t Y: ");
    Serial.print(deltaY);
  Serial.print("\t Range: ");
  if (rangeV > 5000) {
    Serial.print("N/A");
  } else {
    Serial.print(rangeV);
  }
  //asobi
  //Serial.print("\t"); // Print the value of 
  //Serial.print(cm);

  Serial.print("\n"); 
}

void mbedSerialInit(){
  mbedSerial.begin(57600); // ソフトウェアシリアル初期化
  
}

void sendMbedSerial(char header,int senddata[],uint8_t senddatasize){
    mbedSerial.print(header); // Print the value of
    mbedSerial.print(senddata[0]);
    for(int i=1;i<senddatasize;i++){
      mbedSerial.print(",");
      mbedSerial.print(senddata[i]);
    }
    mbedSerial.print("\n"); // Print the value of 
}

void idou(int zengo,int sayuu,int jouge,int senkai){
  int senddata[4]={zengo,sayuu,jouge,senkai}; 
  sendMbedSerial(arm_status,senddata,4);
}

//void chuugaeri(int rotate_again_num){
//  sendMbedSerial('c',rotate_again_num/*yabasou*/,1);
//}

void rc_csc_check(int ch[CH_NUM]){
  static unsigned long thrDwnHoldChk=0;
  static unsigned long thrTimeStamp=0;
  thrTimeStamp=millis();
  static unsigned long thrTimeStamp_old=thrTimeStamp;
  static unsigned long flightTm_start=0;
  unsigned long flightTm=millis()-flightTm_start;
  
  if(ch[RIGHT_VERTICAL]<-95&&ch[RIGHT_HORIZONTAL]>95&&ch[LEFT_VERTICAL]<-95&&ch[LEFT_HORIZONTAL]<-95){
    arm_status=ARM;
    flightTm_start=millis();
  }else if(ch[RIGHT_VERTICAL]<-95&&ch[RIGHT_HORIZONTAL]<-95&&ch[LEFT_VERTICAL]<-95&&ch[LEFT_HORIZONTAL]>95){
    arm_status=DISARM;
  }else if(ch[RIGHT_VERTICAL]<-95){
    if(flightTm>=5000){
      thrDwnHoldChk+=(thrTimeStamp-thrTimeStamp_old);
    }
  }else{
    if(thrDwnHoldChk>=(thrTimeStamp-thrTimeStamp_old))thrDwnHoldChk-=(thrTimeStamp-thrTimeStamp_old);
  }
  
  if(thrDwnHoldChk>=2000){
    thrDwnHoldChk=0;
    arm_status=DISARM;
  }
  
  thrTimeStamp_old=thrTimeStamp;
}
void disarm(){
  arm_status=DISARM;
  sendMbedSerial(DISARM,0,1);
}

void poscontrol(int ch[CH_NUM],int outV[CH_NUM]){
  for(uint8_t ch_count=0;ch_count<CH_NUM;ch_count++){
    outV[ch_count]=ch[ch_count];
  }
  
  static float targetV=rangeV;
  targetV+=+GAINZ*ch[ RIGHT_VERTICAL];
  outV[ RIGHT_VERTICAL]=(int)(PZ*(targetV-rangeV));//目標高度ー現在高度
  for(uint8_t ch_count=0;ch_count<CH_NUM;ch_count++){
    if(outV[ch_count]>100){
      outV[ch_count]=100;
    }else if(outV[ch_count]<-100){
      outV[ch_count]=-100;
    }
  }
}

/*
int ultraSonicReadCenti(){
  pinMode(ULTRASONIC_PIN, OUTPUT);
  digitalWrite(ULTRASONIC_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(ULTRASONIC_PIN,LOW);
  pinMode(ULTRASONIC_PIN,INPUT);
  long duration;
  duration = pulseIn(ULTRASONIC_PIN,HIGH);
  if(duration<0||duration>25000){
    return -1;
  }
  int RangeInCentimeters;
  RangeInCentimeters = duration/29/2;
  return RangeInCentimeters;
}*/
/*
void ultraSonicInit(){
  ultraSonicReadCenti();
}*/



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

