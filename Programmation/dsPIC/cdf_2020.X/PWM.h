/* 
 * File: PWM.h  
 * Author: Quentin BOYER
 * Comments: 
 * Revision history: 1
 */

#ifndef PWM_H
#define	PWM_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdint.h>
#include "timer.h"
#include "constant.h"

void initPWM();
void initPWM1();
void initPWM2();
void initPWM3();
void initPWM4();
void initPWM5();
void initPWM6();

void testServo();
void servoUs(unsigned char servo, double value);
void motorVoltage(uint8_t id, double voltage);
void sendToMotor(double valueR, double valueL);

void testSendToMotor(double valueR, double valueL);


#endif	/* PWM_H */