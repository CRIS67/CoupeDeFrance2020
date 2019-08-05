/*
 * File:   timer.c
 * Author: Quentin BOYER
 *
 * Created on 19 septembre 2017, 17:17
 */

#include "timer.h"

// <editor-fold defaultstate="collapsed" desc="Variables">
extern volatile long double x;
extern volatile long double y;
extern volatile long double theta;

extern volatile long double xc;
extern volatile long double yc;
extern volatile long double thetac;

extern volatile long double xf;
extern volatile long double yf;
extern volatile long double tf;

volatile long double kahanX = 0;
volatile long double kahanY = 0;
volatile long double kahanT = 0;

extern volatile long double kahanErrorX;
extern volatile long double kahanErrorY;
extern volatile long double kahanErrorT;

extern uint8_t finalPoint;

extern volatile double receivedX, receivedY, receivedTheta;

extern volatile char US_ON[NB_US];
extern volatile char US_R[NB_US];
extern volatile double US[NB_US];

unsigned int n, n2;

extern volatile PID pidSpeedLeft, pidSpeedRight, pidDistance, pidAngle;

double volatile prevCommandeL;
double volatile prevCommandeR;

double volatile prevPrevCommandeL;
double volatile prevPrevCommandeR;


int16_t volatile speedLSum;
int16_t volatile speedRSum;

double volatile smoothSpeedL;
double volatile smoothSpeedR;

//extern volatile char arrived;
//extern volatile char arrived_2;

volatile double d;

unsigned char i_us, n_us;

extern volatile char sendBT;
extern volatile unsigned char debugPosRpiAsserv;

extern volatile unsigned char stop;

extern unsigned char detectUS;
extern unsigned char sensDetectUS;
volatile unsigned char hUS = 0;

volatile unsigned char US_received = 0;

volatile unsigned char back = 0;

double var_ACC_MAX = ACC_MAX;

unsigned char timer2Overflow = 0;

long double coef_dissymmetry = COEF_DISSYMETRY;
long double mm_per_ticks = MM_PER_TICKS;
long double rad_per_ticks = RAD_PER_TICKS;
long double distance_between_encoder_wheels = DISTANCE_BETWEEN_ENCODER_WHEELS;

volatile uint8_t nearPointDistance = 0;
volatile uint8_t nearPointAngle = 0;

volatile uint16_t nplot = 0;

extern volatile uint8_t arrived;
extern volatile uint8_t trajMode;
extern uint8_t cmdTraj;
volatile uint8_t countTransition = 0;
extern volatile uint8_t directionTraj;

volatile long double distanceMax = 10;  //The robot is arrived at its destination if its distance to the destination point is less than this value
// <editor-fold defaultstate="collapsed" desc="Trajectory generation">
unsigned char statePathGeneration;
unsigned char stateTrap;

double cx, cy, ct;

double sign;

double phi;
double angle;

double theta0;
double angularVelocity;
double prevAngularVelocity;
double maxAngularVelocity;

double AngularAcceleration;

double angle1;


double dx;
double dy;
double alpha;

double totalDistance;
double dist;

double y_0;
double x_0;
double speed;
double precSpeed;

double acc;
double speedMax;

double dist1;
// </editor-fold>
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Init">
void initTimer() {
    initTimer1();
    initTimer2();
    //initTimer3();
    initTimer4();
    initTimer5();

    prevCommandeL = 0;
    prevCommandeR = 0;

    speedLSum = 0;
    speedRSum = 0;
    smoothSpeedL = 0;
    smoothSpeedR = 0;

    d = 0;
    i_us = 0;
    n_us = 0;
}
void initTimer1() { //Timer 1 -> asserv' interrupt
    T1CONbits.TON = 0; //disable timer
    TMR1 = 0; // Clear timer register
    PR1 = 1094; //10937;     //period 10 937 -> ~10ms
    T1CONbits.TSIDL = 0; //Continues module operation in Idle mode
    T1CONbits.TCS = 0; //internal clock
    T1CONbits.TCKPS = 0b10; //prescaler : 1:64
    T1CONbits.TGATE = 0; //Gated time accumulation is disabled
    IFS0bits.T1IF = 0; //Clear flag
    IEC0bits.T1IE = 1; //Enable interrupt
    T1CONbits.TON = 1; //enable Timer1
}
void initTimer2() { //Timer 2 -> timing (delay_ms,delay_us,millis,micros)
    /*32bits mode*/
    /*TMR2 = LSB & TMR3 = MSB*/
    /*period PR2 = LSB & PR3 = MSB*/

    T2CONbits.TON = 0; //disable timer
    TMR2 = 0; // Clear timer register
    TMR3 = 0;
    /*Fp = 70Mhz -> 1tick = (100/7)ns -> 4200000000tick = 60s -> 4199999999 = 0xFA56E9FF */
    PR2 = 0xE9FF; //Period value
    PR3 = 0xFA56;
    T2CONbits.TSIDL = 0; //Continues module operation in Idle mode
    T2CONbits.TCS = 0; //internal clock
    //T2CONbits.TCKPS = 0b10; //prescaler : 1:64
    T2CONbits.TCKPS = 0b00; //prescaler : 1:1
    //T2CONbits.TCKPS = 0b01; //prescaler : 1:8
    T2CONbits.TGATE = 0; //Gated time accumulation is disabled
    T2CONbits.T32 = 1; //Timer2 & Timer 3 form a single 32-bit timer
    IFS0bits.T2IF = 0; //Clear flag
    //IEC0bits.T2IE = 1;      //Enable interrupt
    IEC0bits.T2IE = 0;

    IFS0bits.T3IF = 0; //Clear flag
    IEC0bits.T3IE = 1;

    T2CONbits.TON = 1; //enable Timer1
}
void initTimer3() { //Timer 3   -> 20µs delay
    /*T3CONbits.TON = 0;      //disable timer
    TMR3 = 0;               // Clear timer register
    PR3 = 1400;           //Period value
    T3CONbits.TSIDL = 0;    //Continues module operation in Idle mode
    T3CONbits.TCS = 0;      //internal clock
    T3CONbits.TCKPS = 0b00; //prescaler : 1:1
    T3CONbits.TGATE = 0;    //Gated time accumulation is disabled
    IFS0bits.T3IF = 0;      //Clear flag
    IEC0bits.T3IE = 1;      //Enable interrupt
    T3CONbits.TON = 0;      //disable Timer3*/
}
void initTimer4() { //Timer 4   -> count US time
    T4CONbits.TON = 0; //disable timer
    TMR4 = 0; // Clear timer register
    PR4 = 0xFFFF; //Period value
    T4CONbits.TSIDL = 0; //Continues module operation in Idle mode
    T4CONbits.TCS = 0; //internal clock
    T4CONbits.TCKPS = 0b10; //prescaler : 1:64
    T4CONbits.TGATE = 0; //Gated time accumulation is disabled
    IFS1bits.T4IF = 0; //Clear flag
    IEC1bits.T4IE = 0; //Enable interrupt
    T4CONbits.TON = 1; //enable Timer4
}
void initTimer5() { //Timer 5   -> 20µs delay
    T5CONbits.TON = 0; //disable timer
    TMR5 = 0; // Clear timer register
    PR5 = 1400; //Period value
    T5CONbits.TSIDL = 0; //Continues module operation in Idle mode
    T5CONbits.TCS = 0; //internal clock
    T5CONbits.TCKPS = 0b00; //prescaler : 1:1
    T5CONbits.TGATE = 0; //Gated time accumulation is disabled
    IFS1bits.T5IF = 0; //Clear flag
    IEC1bits.T5IE = 1; //Enable interrupt
    T5CONbits.TON = 0; //disable Timer5
}
// </editor-fold>
double myCos(double x);
// <editor-fold defaultstate="collapsed" desc="Timer interrupts">
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0; //Clear Timer1 interrupt flag
    // <editor-fold defaultstate="collapsed" desc="Calcul position ">
    
    //save value
    int16_t ticksL = POS1CNTL;
    int16_t ticksR = POS2CNTL;
    //reset value
    POS1CNTL = 0x0000;
    POS2CNTL = 0x0000;
    
    /*test Kahan*/
    /*ticksL = 10;
    ticksR = 10;*/
    
    speedLSum += ticksL;
    speedRSum += ticksR;
    
    long double deltaDL = ticksL * coef_dissymmetry;
    long double deltaDR = ticksR;
    
    /*deltaDL = deltaDL * mm_per_ticks * coef_dissymmetry;   //Ticks -> mm
    deltaDR = deltaDR * mm_per_ticks;*/
    
    long double deltaD = (deltaDL + deltaDR) * mm_per_ticks / 2;
    //long double deltaT = (deltaDR - deltaDL) / distance_between_encoder_wheels;
    long double deltaT = (deltaDR - deltaDL) * rad_per_ticks;

    theta += deltaT;
    x += deltaD * cosl(theta);
    y += deltaD * sinl(theta);
    /*test Kahan*/
    //deltaD = 1;
    //deltaT = 0.002;
    
    // <editor-fold defaultstate="collapsed" desc="Kahan summation algorithm">
    /*long double yT = deltaT - kahanT;
    long double tT = theta + yT;
    kahanT = (tT - theta) - yT;
    theta = tT;

    //theta += deltaT;
    long double deltaX = deltaD * cosl(theta);
    long double deltaY = deltaD * sinl(theta);
    
    long double yX = deltaX - kahanX;
    long double tX = x + yX;
    kahanX = (tX - x) - yX;
    x = tX;
    
    long double yY = deltaY - kahanY;
    long double tY = y + yY;
    kahanY = (tY - y) - yY;
    y = tY;*/
    
    /*long double absError = kahanX;
    if(absError < 0)
        absError = -absError;
    kahanErrorX += absError;
    
    absError = kahanY;
    if(absError < 0)
        absError = -absError;
    kahanErrorY += absError;
    
    absError = kahanT;
    if(absError < 0)
        absError = -absError;
    kahanErrorT += absError;*/
    
    //x += deltaD * cos(theta);
    //y += deltaD * sin(theta);
    // </editor-fold>
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Asservissement">
    n++;
    if (n >= N_ASSERV) {//1000
        n = 0;

        long double speedL = speedLSum; //  /0.01 -> 10ms
        long double speedR = speedRSum;

        speedL = speedL * mm_per_ticks * 1000 / N_ASSERV;
        speedR = speedR * mm_per_ticks * 1000 / N_ASSERV;

        speedLSum = 0;
        speedRSum = 0;
        
        //smoothSpeedL = smoothSpeedL + SMOOTHING_FACTOR * (speedL - smoothSpeedL);
        //smoothSpeedR = smoothSpeedR + SMOOTHING_FACTOR * (speedR - smoothSpeedR);
        /*int i;
        for(i = 1; i < N_SMOOTHING; i++){
            smoothSpeedL = smoothSpeedL + SMOOTHING_FACTOR * (speedL - smoothSpeedL);
            smoothSpeedR = smoothSpeedR + SMOOTHING_FACTOR * (speedR - smoothSpeedR);
        }*/

        double thetaRobotPoint = atan2(yc - y, xc - x);         //angle (dans le repère absolu) que le robot doit avoir pour êter dirigé vers le point de consigne
        double thetaRobotPointFinal = atan2(yf - y, xf - x);    //angle (dans le repère absolu) que le robot doit avoir pour êter dirigé vers le point final de la trajectoire
        if (back)                                               //si la manoeuvre doit se faire en marche arrière
            thetaRobotPointFinal -= PI;
        while (thetaRobotPointFinal - (double)theta < -PI)
            thetaRobotPointFinal += 2 * PI;
        while (thetaRobotPointFinal - (double)theta > PI)
            thetaRobotPointFinal -= 2 * PI;
        if (thetaRobotPointFinal - (double)theta < -PI / 2)
            thetaRobotPointFinal += PI;
        if (thetaRobotPointFinal - (double)theta > PI / 2)
            thetaRobotPointFinal -= PI;
        double distFinal = sqrt((x - xf)*(x - xf) + (y - yf)*(y - yf));

        double errorD = sqrt((x - xc)*(x - xc) + (y - yc)*(y - yc));

        //projection de l'erreur
        errorD = -errorD * myCos(thetaRobotPoint - (double)theta);
        //plot(1,(uint32_t)(int32_t)(1000*myCos(thetaRobotPoint - (double)theta)));
        //plot(2,(uint32_t)(int32_t)(1000*errorD));
        
        // <editor-fold defaultstate="collapsed" desc="gestion marche arrière">
        /*double thetaDiff = (double)theta - thetaRobotPoint;
        while (thetaDiff > PI) {
            thetaDiff -= 2 * PI;
        }
        while (thetaDiff < -PI) {
            thetaDiff += 2 * PI;
        }
        if (thetaDiff > -PI / 2 && thetaDiff < PI / 2) {
            errorD = -errorD;
        }*/
        // </editor-fold>
        

        if ( (distFinal > DIST_AIM_POINT || distFinal < -DIST_AIM_POINT) && (trajMode == TRAJ_MODE_LIN))
            setSetPoint(&pidAngle, thetaRobotPointFinal);
        else
            setSetPoint(&pidAngle, thetac);
        //setSetPoint(&pidAngle,thetac);

        double rD = compute(&pidDistance, errorD);
        double rA = compute(&pidAngle, (double)theta);

        /*if (pidDistance.prevError < MAX_ERROR_D && pidDistance.prevError > -MAX_ERROR_D && distFinal < MAX_ERROR_D && (finalPoint == 1)) {
            rD = 0;
        }*/
        
        // "hystheresis"
        if(distFinal < MAX_ERROR_D_INF){
            nearPointDistance = 1;
        }
        else if(distFinal > MAX_ERROR_D_SUP){
            nearPointDistance = 0;
        }
        if(pidAngle.prevError < MAX_ERROR_A_INF && pidAngle.prevError > -MAX_ERROR_A_INF){
            nearPointAngle = 1;
        }
        else if(pidAngle.prevError > MAX_ERROR_A_SUP && pidAngle.prevError < -MAX_ERROR_A_SUP){
            nearPointAngle = 0;
        }
        if(nearPointDistance && finalPoint){
            rD = 0;
            if(nearPointAngle){
                rA = 0;
            }
        }
        
        
        /*if(pidAngle.prevError < MAX_ERROR_A && pidAngle.prevError > -MAX_ERROR_A){
            rA = 0;
        }*/

        setSetPoint(&pidSpeedLeft, rD - rA);
        setSetPoint(&pidSpeedRight, rD + rA);

        /*setSetPoint(&pidSpeedLeft, xc);
        setSetPoint(&pidSpeedRight, xc);*/
        
        double commandeL = compute(&pidSpeedLeft, speedL);
        double commandeR = compute(&pidSpeedRight, speedR);

        if (pidDistance.prevError < MAX_ERROR_D && pidDistance.prevError > -MAX_ERROR_D && pidAngle.prevError < MAX_ERROR_A && pidAngle.prevError > -MAX_ERROR_A && speedL < MAX_SPEED_STOP && speedL > -MAX_SPEED_STOP && speedR < MAX_SPEED_STOP && speedR > -MAX_SPEED_STOP && (finalPoint == 1)) {
            commandeL = 0;
            commandeR = 0;
            pidSpeedLeft.sumI = 0;
            pidSpeedLeft.sumI = 0;
            arrived = 1;
        } else
            arrived = 0;


        /*if(pidDistance.prevError < MAX_ERROR_D && pidDistance.prevError > -MAX_ERROR_D && pidAngle.prevError < MAX_ERROR_A && pidAngle.prevError > -MAX_ERROR_A && speedL < MAX_SPEED_STOP && speedL > -MAX_SPEED_STOP && speedR < MAX_SPEED_STOP && speedR > -MAX_SPEED_STOP){
            commandeL = 0;
            commandeR = 0;
            pidSpeedLeft.sumI = 0;
            pidSpeedLeft.sumI = 0;
            arrived = 1;
        }
        else
            arrived = 0;*/

        // <editor-fold defaultstate="collapsed" desc="Limit acceleration">
        /*uint8_t variationR,variationL,prevVariationR,prevVariationL;
        if((commandeR - prevCommandeR) >= 0)
            variationR = 1;
        else
            variationR = 0;
        if((prevCommandeR - prevPrevCommandeR) >= 0)
            prevVariationR = 1;
        else
            prevVariationR = 0;
        
        if((commandeL - prevCommandeL) >= 0)
            variationL = 1;
        else
            variationL = 0;
        if((prevCommandeL - prevPrevCommandeL) >= 0)
            prevVariationL = 1;
        else
            prevVariationL = 0;*/
        //if(variationR != prevVariationR){
        if (commandeR - prevCommandeR > var_ACC_MAX) {
            commandeR = prevCommandeR + var_ACC_MAX;
            //plot(25,-5000);
        } else if (commandeR - prevCommandeR < -var_ACC_MAX) {
            commandeR = prevCommandeR - var_ACC_MAX;
            //plot(25,-4000);
        }
        //}

        //if(variationL != prevVariationL){
        if (commandeL - prevCommandeL > var_ACC_MAX) {
            commandeL = prevCommandeL + var_ACC_MAX;
            //plot(15,-5000);
        } else if (commandeL - prevCommandeL < -var_ACC_MAX) {
            commandeL = prevCommandeL - var_ACC_MAX;
            //plot(15,-4000);
        }
        //}
        // </editor-fold>

        //prevPrevCommandeL = prevCommandeL;
        //prevPrevCommandeR = prevCommandeR;

        prevCommandeL = commandeL;
        prevCommandeR = commandeR;


        if(!stop){
            testSendToMotor(commandeR, commandeL);
        }
        else{
            testSendToMotor(0, 0);
        }
        if(arrived){
            testSendToMotor(0, 0);
        }


        /*if (pidDistance.prevError > 60 || pidDistance.prevError < -60 || pidAngle.prevError > 0.9 || pidAngle.prevError < -0.9 || pidSpeedLeft.prevError > 21 || pidSpeedLeft.prevError < -21 || pidSpeedRight.prevError > 21 || pidSpeedRight.prevError < -21){
            //statePathGeneration = 5;
            //stateTrap = 0;
            //while (1) {
            //    testSendToMotor(0, 0);
            //}
            //testSendToMotor(0, 0);
        }*/
        if (pidSpeedLeft.prevError > 100 || pidSpeedLeft.prevError < -100 || pidSpeedRight.prevError > 100 || pidSpeedRight.prevError < -100){
            //statePathGeneration = 5;
            //stateTrap = 0;
        }
        
        /*plot(31,(uint32_t)((int32_t)(x*1000)));
        plot(32,(uint32_t)((int32_t)(xc*1000)));
        plot(33,(uint32_t)((int32_t)(y*1000)));
        plot(34,(uint32_t)((int32_t)(yc*1000)));*/
       
        // <editor-fold defaultstate="collapsed" desc="Génération de trajectoire">
        /*Génération de trajectoire*/
        if (statePathGeneration != 0) {
            // <editor-fold defaultstate="collapsed" desc="Plots">
            /*plot(1, (uint32_t) (int32_t) (pidDistance.prevError * 100));
            plot(2, (uint32_t) (int32_t) (pidAngle.prevError * 10000));
            plot(3, (uint32_t) (int32_t) (pidSpeedLeft.prevError * 1000));
            plot(4, (uint32_t) (int32_t) (pidSpeedRight.prevError * 1000));*/

            //plot(1,(uint32_t)(int32_t)(thetac*1000));
            //plot(2,(uint32_t)(int32_t)(angularVelocity*1000));
            //plot(3,(uint32_t)(int32_t)(angle*1000));
            //plot(4,(uint32_t)(int32_t)(theta*1000));
            //plot(11,(uint32_t)(int32_t)(speedL*1000));
            //plot(21,(uint32_t)(int32_t)(speedR*1000));
            //plot(31,(uint32_t)(int32_t)(speedR*1000));
            //plot(41,(uint32_t)(int32_t)(speedR*1000));
            //plot(12,(uint32_t)(int32_t)(pidSpeedLeft.setPoint*1000));
            //plot(22,(uint32_t)(int32_t)(pidSpeedRight.setPoint*1000));
            /*
            int32_t pwmR = PWM_R;
            if(SENS_R == 0)
                pwmR = -pwmR;
            int32_t pwmL = PWM_L;
            if(SENS_L == 0)
                pwmL = -pwmL;
            plot(13,(uint32_t)PWM_R);
            plot(23,(uint32_t)PWM_L);
            plot(14,(((uint32_t)SENS_R)*5000)-1000);
            plot(24,(((uint32_t)SENS_L)*5000)-1000);*/

            //plot(31,(((uint32_t)((pidAngle.prevError*180000/PI)+5000))));
            /*plot(31,(uint32_t)(int32_t)(x));
            plot(32,(uint32_t)(int32_t)(y));
            plot(33,(uint32_t)(int32_t)(xc));
            plot(34,(uint32_t)(int32_t)(yc));*/
            /*plot(1,(uint32_t)(int32_t)(pidSpeedLeft.prevSmoothError*1000));
            plot(2,(uint32_t)(int32_t)(pidSpeedLeft.prevError*1000));
            plot(3,(uint32_t)(int32_t)(pidSpeedRight.prevSmoothError*1000));
            plot(4,(uint32_t)(int32_t)(pidSpeedRight.prevError*1000));*/

            /*plot(41,(uint32_t)(int32_t)(theta*100000));
            plot(42,(uint32_t)(int32_t)(thetac*100000));
            plot(43,(uint32_t)(int32_t)(pidAngle.setPoint*100000));*/

            //plot(5,(uint32_t)(int32_t)(statePathGeneration*1000));
            //plot(6,(uint32_t)(int32_t)(stateTrap*1000));// </editor-fold>
        }
        switch (statePathGeneration) {
            // <editor-fold defaultstate="collapsed" desc="Idle">
            case 0:
                break; // </editor-fold>
            // <editor-fold defaultstate="collapsed" desc="Rotation">
            case 1: //1st Rotation
                /*
                Initialize following variables before execution : 
                -theta0 = theta
                -finalPoint = 0;
                -angularVelocity = 0
                -prevAngularVelocity = 0
                -maxAngularVelocity
                -AngularAcceleration
                -angle = 0
                -phi = destination angle (relative to actual robot angle) (MUST BE POSITIVE)
                -sign = 1 for CCW (counter clockwise / trigo) / 0 for CW(clockwise)
                -cmdTraj = CMD_TRAJ_ROT_AND_LIN if rotation & linear motion CMD_TRAJ_ROT if only rotation
                 */
                trajMode = TRAJ_MODE_ROT;
                switch (stateTrap) {
                    case 1: //acceleration
                        if (angularVelocity < maxAngularVelocity && angle < phi / 2) {
                            angularVelocity += AngularAcceleration * TE;
                            angle += TE * (prevAngularVelocity + angularVelocity) / 2;
                            thetac = theta0 + angle * sign;
                            prevAngularVelocity = angularVelocity;
                        } else {
                            stateTrap = 2;

                            angle1 = angle;
                        }
                        break;
                    case 2: //constant speed
                        if (angle < phi - angle1) {
                            angle += TE * angularVelocity;
                            thetac = theta0 + angle * sign;
                            prevAngularVelocity = angularVelocity;
                        } else {
                            stateTrap = 3;

                            AngularAcceleration = -AngularAcceleration;
                        }
                        break;
                    case 3: //deceleration
                        if (angularVelocity > 0 && angle < phi) {
                            angularVelocity += AngularAcceleration * TE;
                            angle += TE * (prevAngularVelocity + angularVelocity) / 2;
                            thetac = theta0 + angle * sign;
                            prevAngularVelocity = angularVelocity;
                        } else {
                            trajMode = TRAJ_MODE_LIN;
                            if(cmdTraj == CMD_TRAJ_ROT_AND_LIN){ // next state is linear motion
                                statePathGeneration = 2;
                                stateTrap = 1;

                                thetac = theta0 + phi*sign;
                                xf = cx;
                                yf = cy;
                                tf = ct;
                                y_0 = y;
                                x_0 = x;
                                dx = cx - x_0;
                                dy = cy - y_0;
                                alpha = atan2(dy, dx);
                                totalDistance = sqrt(dx * dx + dy * dy);
                            }
                            else{//CMD_TRAJ_ROT : next state is idle
                                statePathGeneration = 0;
                                stateTrap = 1;
                                thetac = theta0 + phi*sign;
                                
                                //xc = cx;
                                //yc = cy;

                                finalPoint = 1;
                            }

                        }
                        break;
                }
                break; // </editor-fold>
            // <editor-fold defaultstate="collapsed" desc="Translation">
            case 2: //Translation
                //trajMode = TRAJ_MODE_LIN;
                switch (stateTrap) {
                    case 1: //acceleration
                        if (speed < speedMax && dist < totalDistance / 2) {
                            speed += acc * TE;
                            dist += TE * (precSpeed + speed) / 2;
                            xc = x_0 + dist * cos(alpha);
                            yc = y_0 + dist * sin(alpha);
                            precSpeed = speed;
                        } else {
                            stateTrap = 2;

                            dist1 = dist;
                            if (speed > speedMax)
                                speed = speedMax;
                        }
                        break;
                    case 2: //constant speed
                        if (dist < totalDistance - dist1) { //Condition
                            dist += TE * speed;
                            xc = x_0 + dist * cos(alpha);
                            yc = y_0 + dist * sin(alpha);
                            precSpeed = speed;
                        } else {
                            stateTrap = 3;

                            acc = -acc;
                        }
                        break;
                    case 3: //deceleration
                        if (speed > 0 && dist < totalDistance) { //Condition		//v > 0			/		d < totalDistance
                            speed += acc * TE;
                            dist += TE * (precSpeed + speed) / 2;
                            xc = x_0 + dist * cos(alpha);
                            yc = y_0 + dist * sin(alpha);
                            precSpeed = speed;
                        } else {
                            statePathGeneration = 0;
                            stateTrap = 1;

                            xc = cx;
                            yc = cy;

                            finalPoint = 1;

                        }
                        break;

                }
                break; // </editor-fold>
            // <editor-fold defaultstate="collapsed" desc="Transition">
            case 3: //transition go
                countTransition = 0;
                xc = x;
                yc = y;
                xf = x;
                yf = y;
                thetac = theta;
                finalPoint = 1;
                trajMode = TRAJ_MODE_LIN;
                statePathGeneration = 4;
                break;
            case 4:
                if(countTransition < 50){
                    countTransition++;
                    //testSendToMotor(0,0);
                }
                else{
                    finalPoint = 0;
                    theta0 = theta;
                    phi = atan2(cy-y,cx-x) - theta;
                    if(directionTraj == BACKWARD){
                        phi -= PI;
                    }
                    while(phi < -PI)
                        phi += 2*PI;
                    while(phi > PI){
                        phi -= 2*PI;
                    }
                    if(phi > 0)
                        sign = 1;
                    else{
                        sign = -1;
                        phi = -phi;
                    }
                    xf = x;
                    yf = y;
                    statePathGeneration = 1;
                }
                break;
            case 5: //transition go
                countTransition = 0;
                xc = x;
                yc = y;
                xf = x;
                yf = y;
                thetac = theta;
                finalPoint = 1;
                trajMode = TRAJ_MODE_LIN;
                statePathGeneration = 6;
                break;
            case 6:
                if(countTransition < 50){
                    countTransition++;
                    //testSendToMotor(0,0);
                }
                else{
                    statePathGeneration = 0;
                }
                break;// </editor-fold>

        }
        // </editor-fold>
    }// </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="US">
    /*n_us++;
    if (n_us >= N_US) {
        n_us = 0;
        if (!US_received)
            US[i_us] = 9999;
        i_us++;
        if (i_us >= NB_US)
            i_us = 0;
        switch (i_us) {
            case 0:
                TRIG_US_0 = 1;
                break;
            case 1:
                TRIG_US_1 = 1;
                break;
            case 2:
                TRIG_US_2 = 1;
                break;
            case 3:
                TRIG_US_3 = 1;
                break;
            case 4:
                TRIG_US_4 = 1;
                break;
            case 5:
                TRIG_US_5 = 1;
                break;
        }
        unsigned char i = 0;
        for (i = 0; i < NB_US; i++) {
            US_ON[i] = 0;
            US_R[i] = 0;
        }
        US_received = 0;
        US_ON[i_us] = 1;
        T5CONbits.TON = 1; //enable Timer3
        //print("toggle ");
        //print(itoa((int)i_us));
        //print("\n");
    }*/
    // </editor-fold>
}

/*void __attribute__((interrupt,no_auto_psv)) _T2Interrupt(void){
    IFS0bits.T2IF = 0; //Clear Timer1 interrupt flag
}*/
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0; //Clear Timer1 interrupt flag
    timer2Overflow++;
}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void) {// _T3Interrupt(void){
    IFS1bits.T5IF = 0;
    T5CONbits.TON = 0; //disable Timer3
    TMR5 = 0; //reset Timer3
    TRIG_US_0 = 0;
    TRIG_US_1 = 0;
    TRIG_US_2 = 0;
    TRIG_US_3 = 0;
    TRIG_US_4 = 0;
    TRIG_US_5 = 0;
}
// </editor-fold>


uint32_t micros(){
    IEC0bits.T3IE = 0;  //disable interrupt on timer 3 overflow
    uint32_t saveTMR2 = TMR2;
    uint32_t ret = TMR3;
    ret = ret << 16;
    ret = ret + saveTMR2;
    ret = ret / 70;
    uint32_t t2of = timer2Overflow;
    t2of = t2of * 60000000UL;
    ret = ret + t2of;
    IEC0bits.T3IE = 1;  //enable interrupt on timer 3 overflow
    return ret;
}
uint32_t millis(){
    IEC0bits.T3IE = 0;  //disable interrupt on timer 3 overflow
    uint32_t saveTMR2 = TMR2;
    uint32_t ret = TMR3;
    ret = ret << 16;
    ret = ret + saveTMR2;
    ret = ret / 70000; //140MHz
    //ret = ret / 3684;   //7.3728Mhz
    uint32_t t2of = timer2Overflow;
    t2of = t2of * 60000UL;    //140Mhz
    //t2of = t2of * 3157;     //7.3728Mhz
    ret = ret + t2of;
    IEC0bits.T3IE = 1;  //enable interrupt on timer 3 overflow
    return ret;
}
void delay_us(uint32_t delay){
   uint32_t tick = micros();
   while(micros() - tick < delay);
}
void delay_ms(uint32_t delay){
    uint32_t tick = millis();
    while(millis() - tick < delay);
}


double myCos(double x){
  if(x < 0)
    x = -x;
  while(x > 2*PI)
      x-= 2*PI;
  double res=0;
  double term=1;
  int k=0;
  while (res+term!=res){
    res+=term;
    k+=2;
    term*=-x*x/k/(k-1);
  }  
  return res;
}