/*
 * RoboPeak RPLIDAR Arduino Example
 * This example shows the easy and common way to fetch data from an RPLIDAR
 * 
 * You may freely add your application code based on this template
 *
 * USAGE:
 * ---------------------------------
 * 1. Download this sketch code to your Arduino board
 * 2. Connect the RPLIDAR's serial port (RX/TX/GND) to your Arduino board (Pin 0 and Pin1)
 * 3. Connect the RPLIDAR's motor ctrl pin to the Arduino board pin 3 
 */
 
/* 
 * Copyright (c) 2014, RoboPeak 
 * All rights reserved.
 * RoboPeak.com
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
// This sketch code is based on the RPLIDAR driver library provided by RoboPeak
#include "RPLidar.h"
#include <SPI.h>
// You need to create an driver instance 
RPLidar lidar;

void(*resetFunc)(void) = 0; //declare reset function @ address 0

#define RPLIDAR_MOTOR 3 // The PWM pin for control the speed of RPLIDAR's motor.
                        // This pin should connected with the RPLIDAR's MOTOCTRL signal 
#define LED1  A0
#define LED2  A1

#define SIZE_BUFFER_RX  100
#define SIZE_BUFFER_TX  600

//#define SIZE_BUFFER_DETECTION  100

#define SIZE_BUFFER_DETECTION  30   //30 * 8 octets -> 240 (attention à la taille limite d'un message : 255 octets
#define SIZEOF_STRUCT_FPOINT   8

#define SIZE_DATA_8b        3
#define SIZE_DATA_16b       4

#define CODE_SEND_DATA_8b   1
#define CODE_SEND_DATA_16b  2

#define CODE_VAR_DISTANCE   1
#define CODE_VAR_ANGLE      2

#define LIDAR_CMD_DEBUG               1
#define LIDAR_CMD_START               2
#define LIDAR_CMD_STOP                3
#define LIDAR_CMD_GET_DISTANCE        4
#define LIDAR_CMD_GET_ANGLE           5
#define LIDAR_CMD_GET_STARTBIT        6
#define LIDAR_CMD_GET_QUALITY         7
#define LIDAR_CMD_SET_AUTOSTART       8
#define LIDAR_CMD_RESET_AUTOSTART     9

#define LIDAR_CMD_GET_DATA_AVAILABLE    10
#define LIDAR_CMD_GET_DETECTED_POINTS   11
#define LIDAR_CMD_GET_RAW_POINT         12

#define LIDAR_CMD_SET_SPEED             20
#define LIDAR_CMD_GET_SPEED             21

#define CMD_RST                         32
#define CMD_PING                        33
#define MSG_NON_PRIS_EN_CHARGE          41

#define LIDAR_RET_DEBUG_DEBUG       42 
#define LIDAR_RET_DEBUG_START       43 
#define LIDAR_RET_DEBUG_STOP        44
#define LIDAR_RET_DATA_AVAILABLE    100
#define LIDAR_RET_DETECTED_POINTS   101
#define LIDAR_RET_RAW_POINT         102

#define LIDAR_RET_SPEED             121

struct fpoint { //point with x,y float
  float x;
  float y;
};

/*struct circularBuffer { //point with x,y float
  uint8_t *buf;
  uint8_t iIn = 0;
  uint8_t iOut = 0;
};*/

volatile unsigned char bufferRx[SIZE_BUFFER_RX];
volatile unsigned char iRx = 0;

volatile unsigned char bufferTx[SIZE_BUFFER_TX];
volatile unsigned char iTx = 0;  //index octet à envoyer
volatile unsigned char iTx2 = 0; //index premier octet libre du tableau  (pour ajouter un nouvel octet)

volatile unsigned long timeLed;
volatile unsigned long timeRotation = 0;
volatile int ledState = 0;

volatile unsigned char flagErrorCS = 0;

unsigned char flagStart = 0;
unsigned char flagStop = 0;
unsigned char flagSendDetectedPoints = 0;

int started = 0;
int autoStart = 0;

/*Données du dernier point mesuré*/
volatile float distance = 0;
volatile float angle = 0;
volatile bool  startBit = false;
volatile byte  quality = 0;

//int xm = 0; //xMesuré
//int ym = 0; //YMesuré

/*pose du centre du lidar dans le repère absolu de la piste*/
/*Ces données sont envoyées régulièrement  par la RPi*/
/*unités :  x et y : [mm] | t : [rad]*/
volatile int x = 0;
volatile int y = 0;
volatile int t = 0;

volatile int xLimInf = 0;
volatile int xLimSup = 2000;
volatile int yLimInf = 0;
volatile int yLimSup = 3000;

struct fpoint DetectedBuf[SIZE_BUFFER_DETECTION];
volatile uint8_t iDetectIn = 0;
volatile uint8_t iDetectOut = 0;

volatile uint8_t rotationSpeed = 255;
volatile float measuredSpeed = 42;

float lastAngle = 0;

uint8_t nbDetectedPoints = 0;

void setup() {
  // bind the RPLIDAR driver to the arduino hardware serial
  lidar.begin(Serial);
  
  // set pin modes
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(RPLIDAR_MOTOR, OUTPUT);

  pinMode(MISO, OUTPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);
  pinMode(SS, INPUT);

  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,LOW);

  SPCR = _BV(SPE);  // turn on SPI in slave mode
  SPCR |= _BV(SPIE);  // turn on interrupts
  timeLed = millis();
}



void loop() {
  /*Blink*/
  if(millis() - timeLed > 100){ 
    timeLed = millis();
    if(ledState){
      ledState = 0;
      digitalWrite(LED1,HIGH);
    }
    else{
      ledState = 1;
      digitalWrite(LED1,LOW);
    }
  }
  
  if(started){
    if (IS_OK(lidar.waitPoint())) { //wait for a new measurement or timeOut
      distance = lidar.getCurrentPoint().distance; //distance value in mm unit
      angle    = -lidar.getCurrentPoint().angle; //anglue value in degree     dans la datasheet l'angle est mesuré dans le sens horaire -> *(-1) pour le sens trigo
      startBit = lidar.getCurrentPoint().startBit; //whether this point is belong to a new scan
      quality  = lidar.getCurrentPoint().quality; //quality of the current measurement

      if(distance > 10 && quality >= 5){

        if(angle < lastAngle){  //passage par 0
          if(ledState){
            ledState = 0;
            digitalWrite(LED1,HIGH);
          }
          else{
            ledState = 1;
            digitalWrite(LED1,LOW);
          }
          unsigned long t = millis();
          if(t - timeRotation != 0){
            measuredSpeed = 60000/(t - timeRotation);  //conversion tr/min
          }
          timeRotation = t;
        }
        lastAngle = angle;
  
        /*if(startBit){
          if(ledState){
            ledState = 0;
            digitalWrite(LED1,HIGH);
          }
          else{
            ledState = 1;
            digitalWrite(LED1,LOW);
          }
          unsigned long t = millis();
          if(t - timeRotation != 0){
            measuredSpeed = 60000/(t - timeRotation);  //conversion tr/min
          }
          timeRotation = t;
        }*/
  
        float angleRad = angle / 180 * PI;
        /*float xm = x +  distance * cos(angleRad + t);
        float ym = y + distance * sin(angleRad + t);*/
  
        //perform data processing here... 
        //if(xm > xLimInf && x < xLimSup && ym > yLimInf && y < yLimSup){  //Si le point mesuré est dans la piste
        if(distance < 1000){  //Si le point mesuré est dans la piste
          struct fpoint p;
          p.x = x +  distance * cos(angleRad + t);
          p.y = y + distance * sin(angleRad + t);
    
          DetectedBuf[iDetectIn] = p;
          iDetectIn++;
          if(iDetectIn == SIZE_BUFFER_DETECTION){  //if index > buffer size -> loop back to index 0  (circular buffer)
            iDetectIn = 0;
          }
          nbDetectedPoints++;
          if(nbDetectedPoints > SIZE_BUFFER_DETECTION){
            nbDetectedPoints = SIZE_BUFFER_DETECTION;
          }
        }
      }
      digitalWrite(LED2,HIGH);
    }
    else{
      //started = 0;
      //analogWrite(RPLIDAR_MOTOR, 0);
      digitalWrite(LED2,LOW);
    }
  }
  else if(autoStart){
    analogWrite(RPLIDAR_MOTOR, 0); //stop the rplidar motor
    
    // try to detect RPLIDAR... 
    rplidar_response_device_info_t info;
    if (IS_OK(lidar.getDeviceInfo(info, 100))) {
       started = 1;
       // detected...
       lidar.startScan();
       
       // start motor rotating at max allowed speed
       analogWrite(RPLIDAR_MOTOR, rotationSpeed);
       //delay(1000);
    }
  }
  /*if LIDAR_CMD_START received*/
  if(flagStart){
    flagStart = 0;
    analogWrite(RPLIDAR_MOTOR, 0); //stop the rplidar motor
    
    // try to detect RPLIDAR... 
    rplidar_response_device_info_t info;
    if (IS_OK(lidar.getDeviceInfo(info, 100))) {
       started = 1;
       // detected...
       lidar.startScan();
       
       // start motor rotating at max allowed speed
       analogWrite(RPLIDAR_MOTOR, rotationSpeed);
    }
  }
  /*if LIDAR_CMD_STOP received*/
  if(flagStop){
    digitalWrite(LED2,LOW);
    started = 0;
    flagStop = 0;
    analogWrite(RPLIDAR_MOTOR, 0);
  }
  if(flagSendDetectedPoints){
    flagSendDetectedPoints = 0;
    sendDetectedPoints();
  }
}

ISR (SPI_STC_vect)
{
  bufferRx[iRx] = SPDR; //add received char to buffer
  unsigned char r = 0;
  if(iRx == bufferRx[0]){ //Message complet reçu
    if(bufferRx[0] != 0){ //if size of message > 0
      unsigned char checkSum = 0;
      for(int i = 0; i < iRx;i++){
        checkSum += bufferRx[i];
      }
      if(checkSum != bufferRx[iRx]){
        flagErrorCS = 1;
      }
      else{ //Traitement du message
        switch(bufferRx[1]){  //Commande
          case LIDAR_CMD_DEBUG:
            r = 42;
            sendSPI(&r,1);
            break;
          case LIDAR_CMD_START:
            flagStart = 1;
            r = 43;
            sendSPI(&r,1);
            //SPDR = 0;
            break;
          case LIDAR_CMD_STOP:
            r = 44;
            sendSPI(&r,1);
            flagStop = 1;
            //SPDR = 0;
            break;
          case LIDAR_CMD_GET_DISTANCE:{
            sendInt16((int16_t)distance,CODE_VAR_DISTANCE);
            break;}
          case LIDAR_CMD_GET_ANGLE:
            sendInt16((int16_t)angle,CODE_VAR_ANGLE);
            break;
          case LIDAR_CMD_GET_STARTBIT:
            r = startBit;
            sendSPI(&r,1);
            break;
          case LIDAR_CMD_GET_QUALITY:
            r = quality;
            sendSPI(&r,1);
            break;
          case LIDAR_CMD_SET_AUTOSTART:
            r = 142;
            sendSPI(&r,1);
            autoStart = 1;
            break;
          case LIDAR_CMD_RESET_AUTOSTART:
            r = 143;
            sendSPI(&r,1);
            autoStart = 0;
            break;
          case LIDAR_CMD_GET_DATA_AVAILABLE:{
            uint8_t buf[2];
            buf[0] = LIDAR_RET_DATA_AVAILABLE;
            
            if(iDetectIn == iDetectOut){  //Buffer "DetectedBuf" vide
              buf[1] = 0;
            }
            else{
              buf[1] = 1;
            }
            sendSPI(buf,2);
            break;}
          case LIDAR_CMD_GET_DETECTED_POINTS  :
            flagSendDetectedPoints = 1;
            break;
          case LIDAR_CMD_GET_RAW_POINT:
            sendRawData();
            break;
          case LIDAR_CMD_SET_SPEED:
            rotationSpeed = bufferRx[2];
            if(started){
              analogWrite(RPLIDAR_MOTOR, rotationSpeed);
            }
            break;
          case LIDAR_CMD_GET_SPEED:
            sendSpeed();
            break;
          case CMD_RST:
            resetFunc();
            break;
          case CMD_PING:
            uint8_t buf[3];
            buf[0] = CMD_PING;
            buf[1] = 0;
            buf[2] = 1;
            sendSPI(buf,3);
            break;
          default:
            r = MSG_NON_PRIS_EN_CHARGE;
            sendSPI(r,1);
            /*r = 50;
            sendSPI(&r,1);*/
            break;
        }
      }
    }
    iRx = 0;  //Reset index pour la réception du prochain message
  }
  else{ //Décalage de l'index pour réception du prochain octet
    iRx++;
    if(iRx == SIZE_BUFFER_RX){  //if index > buffer size -> loop back to index 0  (circular buffer)
      iRx = 0;
    }
  }
  if(iTx != iTx2){ //Il reste des octets à envoyer
    SPDR = bufferTx[iTx];
    iTx++;
    if(iTx == SIZE_BUFFER_TX){
      iTx = 0;
    }
  }
  else{ //Rien à envoyer -> 0xff
    SPDR = 0x00;
  }
}
void sendSPI(uint8_t *buf, uint8_t bufSize){  //ajoute taille du message et checksum au buffer puis l'envoie par SPI
  uint8_t checksum = 0;
  bufferTx[iTx2] = bufSize + 2; //message size = size of buffer + 1 byte for size + 1 byte for checksum
  checksum += bufferTx[iTx2];
  iTx2++;
  if(iTx2 == SIZE_BUFFER_TX){
    iTx2 = 0;
  }
  for(int i = 0; i < bufSize; i++){
    bufferTx[iTx2] = buf[i];
    checksum += bufferTx[iTx2];
    iTx2++;
    if(iTx2 == SIZE_BUFFER_TX){
      iTx2 = 0;
    }
  }
  bufferTx[iTx2] = checksum; //message size
  iTx2++;
  if(iTx2 == SIZE_BUFFER_TX){ //if index > buffer size -> loop back to index 0  (circular buffer)
    iTx2 = 0;
  }
}

void sendInt8(int8_t var,uint8_t varCode){
  uint8_t buf[SIZE_DATA_8b];
  buf[0] = CODE_SEND_DATA_8b;
  buf[1] = varCode;
  buf[2] = (uint8_t)(var);
  sendSPI(buf,SIZE_DATA_8b);
}  
void sendInt16(int16_t var, uint8_t varCode){
  unsigned char buf[SIZE_DATA_16b];
  buf[0] = CODE_SEND_DATA_16b;
  buf[1] = varCode;
  buf[2] = (unsigned char)(var >> 8);
  buf[3] = (unsigned char)(var & 0xff);
  sendSPI(buf,SIZE_DATA_16b);
}
void sendDetectedPoints(){  //send
  uint8_t ind = 1;  //le premier point est stocké à l'indice 1 (0-> type de message)
  uint8_t b[SIZE_BUFFER_DETECTION * SIZEOF_STRUCT_FPOINT + 1];
  b[0] = LIDAR_RET_DETECTED_POINTS; //type de message : retour de points mesurés
  while(iDetectIn != iDetectOut){ //tant que le buffer n'est pas vide
  //while(nbDetectedPoints > 0){
    nbDetectedPoints--;
    struct fpoint p = DetectedBuf[iDetectOut];
    iDetectOut++;
    if(iDetectOut == SIZE_BUFFER_DETECTION){  //if index > buffer size -> loop back to index 0  (circular buffer)
      iDetectOut = 0;
    }
    float *fPtr = &p.x;
    uint8_t *intPtr = (uint8_t*)fPtr;
    b[ind] = intPtr[0];
    b[ind + 1] = intPtr[1];
    b[ind + 2] = intPtr[2];
    b[ind + 3] = intPtr[3];

    fPtr = &p.y;
    intPtr = (uint8_t*)fPtr;
    b[ind + 4] = intPtr[0];
    b[ind + 5] = intPtr[1];
    b[ind + 6] = intPtr[2];
    b[ind + 7] = intPtr[3];

    ind+=8;
  }
  sendSPI(b, ind);
}
void sendRawData(){
  uint8_t buf[10];
  buf[0] = LIDAR_RET_RAW_POINT;
  float *fPtr = &distance;
  uint8_t *ptr = (uint8_t*)fPtr;
  buf[1] = ptr[0];
  buf[2] = ptr[1];
  buf[3] = ptr[2];
  buf[4] = ptr[3];
  
  fPtr = &angle;
  ptr = (uint8_t*)fPtr;
  buf[5] = ptr[0];
  buf[6] = ptr[1];
  buf[7] = ptr[2];
  buf[8] = ptr[3];

  buf[9] = quality;
  sendSPI(buf,10);
}
void sendSpeed(){
  uint8_t buf[5];
  buf[0] = LIDAR_RET_SPEED;
  float *fPtr = &measuredSpeed;
  uint8_t *ptr = (uint8_t*)fPtr;
  buf[1] = ptr[0];
  buf[2] = ptr[1];
  buf[3] = ptr[2];
  buf[4] = ptr[3];
  sendSPI(buf,5);
}

