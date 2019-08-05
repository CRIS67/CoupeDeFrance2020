/*
 * File:   PID.c
 * Author: Quentin BOYER
 *
 * Created on 27 octobre 2017, 18:52
 */

#include "PID.h"

void initAllPID(volatile PID *pidSpeedLeft, volatile PID *pidSpeedRight, volatile PID *pidDistance, volatile PID *pidAngle){
    initPID(pidSpeedLeft,KP_SPEED_LEFT,KI_SPEED_LEFT,KD_SPEED_LEFT,BIAS_SPEED_LEFT,0,T_SPEED_LEFT,0,0,SMOOTHING_FACTOR_SPEED_LEFT,SATURATION_SPEED_LEFT);
    initPID(pidSpeedRight,KP_SPEED_RIGHT,KI_SPEED_RIGHT,KD_SPEED_RIGHT,BIAS_SPEED_RIGHT,0,T_SPEED_RIGHT,0,0,SMOOTHING_FACTOR_SPEED_RIGHT,SATURATION_SPEED_RIGHT);
    initPID(pidDistance,KP_DISTANCE,KI_DISTANCE,KD_DISTANCE,BIAS_DISTANCE,0,T_DISTANCE,0,0,SMOOTHING_FACTOR_DISTANCE,SATURATION_DISTANCE);
    initPID(pidAngle,KP_ANGLE,KI_ANGLE,KD_ANGLE,BIAS_ANGLE,0,T_ANGLE,0,0,SMOOTHING_FACTOR_ANGLE,SATURATION_ANGLE);
}

void initPID(volatile PID *pid, long double Kp, long double Ki, long double Kd,long double bias, long double output, long double period, long double processVariable, long double setPoint, long double smoothingFactor, long double saturation){
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->bias = bias; 
    pid->output = output;
    pid->period = period;
    //pid->prevProcessVariable = processVariable;
    pid->processVariable = processVariable;
    pid->setPoint = setPoint;
    pid->sumI = 0;
    pid->prevError = 0;
    pid->prevSmoothError = 0;
    pid->smoothingFactor = smoothingFactor;
    pid->saturation = saturation;
    
    pid->debugCommande = 0;
    pid->debugCommandeP = 0;
    pid->debugCommandeI = 0;
    pid->debugCommandeD = 0;
    return;
}

void setSetPoint(volatile PID *pid, long double setPoint){    //~useless
    pid->setPoint = setPoint;
    //pid->sumI = 0;
}

void setProcessValue(volatile PID *pid, long double processVariable){ //~useless
    pid->processVariable = processVariable;
}

long double compute(volatile PID *pid, long double processVariable){
    
    //int i;
    long double smoothError;
    
    pid->processVariable = processVariable;
    long double error = pid->setPoint - pid->processVariable;
    
    //long double deriv = ((error - pid->prevError)/pid->period);
    /*for(i = 1; i < N_SMOOTHING; i++){
        smoothError = pid->prevSmoothError + SMOOTHING_FACTOR * (error - pid->prevSmoothError);
    }*/
    //smoothError = pid->prevSmoothError + pid->smoothingFactor * (error - pid->prevSmoothError);
    smoothError = pid->prevSmoothError + pid->smoothingFactor * (processVariable - pid->prevSmoothError);
    long double deriv = ((smoothError - pid->prevSmoothError)/pid->period);
    
    pid->sumI = pid->sumI + error * pid->period;
    
    /*if(pid->sumI > MAX_I)
        pid->sumI = MAX_I;
    else if(pid->sumI < -MAX_I)
        pid->sumI = -MAX_I;*/
    
    long double outputP = pid->Kp * error;
    long double outputI = pid->Ki * pid->sumI;
    long double outputD = - pid->Kd * deriv;
    
    pid->output = pid->bias + outputP + outputI + outputD;
            
    if(pid->output > pid->saturation)
        pid->output = pid->saturation;
    else if(pid->output < -pid->saturation)
        pid->output = -pid->saturation;
    
    if(pid->Ki != 0){
        long double outputWithoutI = pid->bias + outputP + outputD;
        if(outputWithoutI > pid->saturation)
            outputWithoutI = pid->saturation;
        else if(outputWithoutI < -pid->saturation)
            outputWithoutI = -pid->saturation;
        pid->sumI = (pid->output - outputWithoutI) / pid->Ki;    //Anti-Windup
    }
        
    
    pid->prevError = error;
    pid->prevSmoothError = smoothError;
    
    pid->debugCommande = pid->output;
    pid->debugCommandeP = outputP;
    pid->debugCommandeI = outputI;
    pid->debugCommandeD = outputD;
    
    return pid->output;
}
/*long double modulo2Pi(long double t){
    while(t > PI)
        t-= PI;
    while(t < -PI)
        t+= PI;
    return t;            
}*/
