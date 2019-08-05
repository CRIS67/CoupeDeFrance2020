/* 
 * File: PID.h  
 * Author: Quentin BOYER
 * Comments: check most appropriate variable type <-> speed of execution 
 *              optimization : remove prevProcessVariable
 * Revision history: 1
 */

#ifndef PID_H
#define	PID_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include "constant.h"

typedef struct PID PID;

struct PID
{
    long double Kp;
    long double Ki;
    long double Kd;
    long double bias;
    long double setPoint;            //consigne
    long double processVariable;     //entree
    long double prevError;
    long double prevSmoothError;
    long double smoothingFactor;
    long double sumI;
    long double period;               //intervalle asserv
    long double output;
    long double saturation;
    long double debugCommande;
    long double debugCommandeP;
    long double debugCommandeI;
    long double debugCommandeD;
};

void initAllPID(volatile PID *pidSpeedLeft, volatile PID *pidSpeedRight, volatile PID *pidDistance, volatile PID *pidAngle);
void initPID(volatile PID *pid, long double Kp, long double Ki, long double Kd,long double bias, long double output, long double period, long double processVariable, long double setPoint, long double smoothingFactor, long double saturation);
void setSetPoint(volatile PID *pid, long double setPoint);
void setProcessValue(volatile PID *pid, long double processVariable);
long double compute(volatile PID *pid, long double processVariable);

#endif	/* PID_H */