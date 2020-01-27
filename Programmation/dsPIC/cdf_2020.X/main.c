/*
 * File:   main.c
 * Author: Quentin BOYER
 *
 * Created on September 18, 2017, 6:59 PM
 */

#include "constant.h"

#include <xc.h>
#include <math.h>
#include <p33EP512GM310.h>
#include <stdint.h>
#include "clock.h"
#include "GPIO.h"
#include "timer.h"
#include "PWM.h"
#include "QEI.h"
#include "PID.h"
#include "SPI.h"
#include "ADC.h"
#include "UART.h"
#include "interrupt.h"
#include "US.h"
#include "AX12.h"
#include "DMA.h"
#include "actuators.h"

void modif_straightPath(double arg_cx, double arg_cy, double arg_speedMax, double accMax, uint8_t direction);
void testModif_turn(double arg_ct, double arg_angularSpeedMax, double arg_angularAccMax);

//Global variables
//char TX[TX_SIZE];
char RX[RX_SIZE];
char unsigned TX_i;
char unsigned RX_i;
/*Current position*/
volatile long double x;
volatile long double y;
volatile long double theta;
/*Current setpoint position*/
volatile long double xc;
volatile long double yc;
volatile long double thetac;
/*Final setpoint position*/
volatile long double xf;
volatile long double yf;
volatile long double tf;

uint8_t finalPoint = 1;

volatile PID pidSpeedLeft, pidSpeedRight, pidDistance, pidAngle;
int state = 0;

extern volatile char   US_ON[NB_US];
extern volatile char   US_R[NB_US];
extern volatile double US[NB_US];

volatile char sendBT = 0;

extern volatile double receivedX,receivedY,receivedTheta;
extern volatile unsigned char newPosReceived;
extern volatile unsigned char newPosBackReceived;
extern volatile unsigned char newAngleReceived;
extern volatile unsigned char back;

extern volatile unsigned char debugPosRpi;

volatile unsigned char stop = 1;

unsigned char detectUS = 0;
unsigned char sensDetectUS = 1;

extern double var_ACC_MAX;

// <editor-fold defaultstate="collapsed" desc="Génération de trajectoire">
extern unsigned char statePathGeneration;
extern unsigned char stateTrap;

extern double cx, cy, ct;

extern double sign;

extern double phi;
extern double angle;

extern double theta0;
extern double angularVelocity;
extern double prevAngularVelocity;
extern double maxAngularVelocity;

extern double AngularAcceleration;

extern double angle1;


extern double dx;
extern double dy;
extern double alpha;

extern double totalDistance;
extern double dist;

extern double y_0;
extern double x_0;
extern double speed;
extern double precSpeed;

extern double acc;
extern double speedMax;

extern double dist1; // </editor-fold>

extern uint8_t BufferA[8];
extern volatile uint8_t RxDMABuffer[RX_DMA_SIZE];
extern uint16_t start;

extern uint16_t iD2,iF2;

volatile uint8_t verbose = 0;

volatile long double linSpeed = 0; //mm/s
volatile long double linAcc = 0; //mm/s^2
volatile long double rotSpeed = 0; //rad/s
volatile long double rotAcc = 0; //rad/s^2

int sens = 0;

volatile long double kahanErrorX;
volatile long double kahanErrorY;
volatile long double kahanErrorT;

volatile uint8_t arrived = 1;
volatile uint8_t trajMode = TRAJ_MODE_LIN;
uint8_t cmdTraj;
volatile uint8_t directionTraj = 0;

int main(){
    initClock(); //Clock 140 MHz
    initGPIO();
    initPWM();
    initQEI();
    initDMA();
    initUART();
    initAX12();
    initInt();
    initUS();
    initADC();
    initSPI();
    
    x = 0;
    y = 0;
    theta = 0;
    
    xc = 0;
    yc = 0;
    thetac = 0;
    
    xf = 0;
    yf = 0;
    tf = 0;
    
    TX_i = 0;
    RX_i = 0;

    POS1CNTL = 0x0000;
    POS2CNTL = 0x0000;
    
    testSendToMotor(0, 0);
    
    initAllPID(&pidSpeedLeft, &pidSpeedRight, &pidDistance, &pidAngle);
    initTimer();
    //IEC0bits.T1IE = 0;  //stop asserv
    x = 1000;
    y = 1500;
    theta = PI;
    
    xc = x;
    yc = y;
    thetac = theta;
    
    xf = x;
    yf = y;
    tf = theta;
    
    // <editor-fold defaultstate="collapsed" desc="Reset led">
    TRISFbits.TRISF7 = 0;
    LATFbits.LATF7 = 1;
    delay_ms(50);
    LATFbits.LATF7 = 0;
    delay_ms(50);
    LATFbits.LATF7 = 1;
    delay_ms(50);
    LATFbits.LATF7 = 0;
    delay_ms(50); // </editor-fold>

    
    while(1){
        delay_ms(10);
        CheckMessages();
        sendPosLongDouble();
    }
    return 0;
}

void modif_straightPath(double arg_cx, double arg_cy, double arg_speedMax, double accMax, uint8_t direction){
    /*INIT*/
    
    cx = arg_cx;
    cy = arg_cy;
    //ct = arg_ct;
    
    speedMax = arg_speedMax;
    
    double thetaRobotPoint = atan2(cy-y,cx-x);
    phi = thetaRobotPoint - theta;
    if(direction == BACKWARD){
        phi -= PI;
    }
    directionTraj = direction;
    while(phi < -PI)
        phi += 2*PI;
    while(phi > PI){
        phi -= 2*PI;
    }
    /*Phase 1 : rotation */
    xf = x;
    yf = y;
    tf = theta;
    finalPoint = 0;
    theta0 = theta;
    AngularAcceleration = rotAcc;
    maxAngularVelocity = rotSpeed;
    /*AngularAcceleration = 1;
    maxAngularVelocity = 10;*/
    angularVelocity = 0;
    prevAngularVelocity = 0;
    angle = 0;
    
    if(phi > 0)
        sign = 1;
    else{
        sign = -1;
        phi = -phi;
    }
    
    if(speedMax < 0)
        speedMax = 0;
    else if(speedMax > SPEED_MAX)
        speedMax = SPEED_MAX;
    if(accMax < 0)
        accMax = 0;
    else if(accMax > ACCELERATION_MAX)
        accMax = ACCELERATION_MAX;
    
    acc = accMax;
	speed = 0;
	precSpeed = 0;
	dist = 0;
    
    cmdTraj = CMD_TRAJ_ROT_AND_LIN;

    if(statePathGeneration == 0){
    //if(1){
        statePathGeneration = 1;
        stateTrap = 1;
    }
    else{
        statePathGeneration = 3;
        stateTrap = 1;
    }
    
    
}
void testModif_turn(double arg_ct, double arg_angularSpeedMax, double arg_angularAccMax){
    /*INIT*/
    phi = arg_ct - theta;
    /*Phase 1 : rotation */
    finalPoint = 0;
    theta0 = theta;
    AngularAcceleration = arg_angularAccMax;
    maxAngularVelocity = arg_angularSpeedMax;
    angularVelocity = 0;
    prevAngularVelocity = 0;
    angle = 0;
    
    if(phi > 0)
        sign = 1;
    else{
        sign = -1;
        phi = -phi;
    }
    
    if(maxAngularVelocity < 0)
        maxAngularVelocity = 0;
    /*else if(maxAngularVelocity > SPEED_MAX)
        maxAngularVelocity = SPEED_MAX;
    */
    if(AngularAcceleration < 0)
        AngularAcceleration = 0;
    /*else if(accMax > ACCELERATION_MAX)
        accMax = ACCELERATION_MAX;
    */
    
    cmdTraj = CMD_TRAJ_ROT;

    statePathGeneration = 1;
    stateTrap = 1;
}
