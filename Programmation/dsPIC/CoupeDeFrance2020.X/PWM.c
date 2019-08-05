/*
 * File:   PWM.c
 * Author: Quentin BOYER
 *
 * Note : WARNING : PWMLOCK configuration bit must be clear (or add unlock sequences)
 * Created on 19 septembre 2017, 23:34
 */

#include <p33EP512GM310.h>

#include "PWM.h"
#include "ADC.h"
extern uint8_t finalPoint;

void initPWM(){
    initPWM1();
    initPWM2();
    //SENS_ACT_0 = 0;
    initPWM3();
    initPWM4();
    initPWM5();
    initPWM6();
    //PTPER = 7000;   //PRIMARY MASTER TIME BASE PERIOD REGISTER
    //STPER = 43750;      //SECONDARY MASTER TIME BASE PERIOD
    STCON2bits.PCLKDIV = 0b110; //Divide-by-64          SECONDARY MASTER CLOCK DIVIDER
    PTCON2bits.PCLKDIV = 0; //Divide-by-1, maximum PWMx timing resolution
    PTCONbits.PTEN = 1; //PWMx module is enabled
    
    /*servoUs(0,1525);
    servoUs(1,800);
    servoUs(2,2300);
    servoUs(3,1650);
    servoUs(4,1500);
    servoUs(5,1900);
    servoUs(6,900);*/
}

void initPWM1(){           //50Hz for serovomotors
    PHASE1 = 43750;         //Period of PWM1H     Fosc / (F_PWM*PWM_Input_Clock_Prescaler) ex : 140Mhz / 64*(50Hz * 1) = 43750
    SPHASE1 = 43750;        //Period of PWM1L
    PDC1 = 0;               //Duty cycle of PWM1H
    SDC1 = 0;               //Duty cycle of PWM1L
    IOCON1bits.PENH = 1;    //PWMx module controls the PWMxH pin
    IOCON1bits.PENL = 1;    //PWMx module controls the PWMxL pin
    IOCON1bits.PMOD = 0b11; //PWMx I/O pin pair is in the True Independent Output mode
    PWMCON1bits.ITB = 1;    //PHASEx register provides the time base period for this PWMx generator
    PWMCON1bits.MTBS = 1;   //PWMx generator uses the secondary master time base (1/64)
    FCLCON1 = 0x0003;       //Fault input is disabled
}
void initPWM2(){           //20kHz
    PHASE2 = 7000;         //Period of PWM2H     Fosc / (F_PWM*PWM_Input_Clock_Prescaler) ex : 140Mhz / 64*(50Hz * 1) = 43750
    SPHASE2 = 7000;        //Period of PWM2L
    PDC2 = 0;               //Duty cycle of PWM2H
    SDC2 = 0;               //Duty cycle of PWM2L
    IOCON2bits.PENH = 1;    //PWMx module controls the PWMxH pin
    IOCON2bits.PENL = 1;    //PWMx module controls the PWMxL pin
    IOCON2bits.PMOD = 0b11; //PWMx I/O pin pair is in the True Independent Output mode
    PWMCON2bits.ITB = 1;    //PHASEx register provides the time base period for this PWMx generator
    PWMCON2bits.MTBS = 0;   //PWMx generator uses the primary master time base  (1/1)
    FCLCON2 = 0x0003;       //Fault input is disabled
}
void initPWM3(){           //312.5Hz
    PHASE3 = 7000;         //Period of PWM3H     Fosc / (F_PWM*PWM_Input_Clock_Prescaler) ex : 140Mhz / 64*(50Hz * 1) = 43750
    SPHASE3 = 7000;        //Period of PWM3L
    PDC3 = 0;               //Duty cycle of PWM3H
    SDC3 = 0;               //Duty cycle of PWM3L
    IOCON3bits.PENH = 1;    //PWMx module controls the PWMxH pin
    IOCON3bits.PENL = 1;    //PWMx module controls the PWMxL pin
    IOCON3bits.PMOD = 0b11; //PWMx I/O pin pair is in the True Independent Output mode
    PWMCON3bits.ITB = 1;    //PHASEx register provides the time base period for this PWMx generator
    PWMCON4bits.MTBS = 1;   //PWMx generator uses the secondary master time base (1/64)
    //PWMCON2bits.MTBS = 0;   //PWMx generator uses the primary master time base  (1/1)
    FCLCON3 = 0x0003;       //Fault input is disabled
}
void initPWM4(){           //50Hz for serovomotors
    PHASE4 = 109;           //Period of PWM4H     Fosc / (F_PWM*PWM_Input_Clock_Prescaler) ex : 140Mhz / 64*(50Hz * 1) = 43750
    SPHASE4 = 43750;        //Period of PWM4L
    PDC4 = 0;               //Duty cycle of PWM4H
    SDC4 = 0;               //Duty cycle of PWM4L
    IOCON4bits.PENH = 1;    //PWMx module controls the PWMxH pin
    IOCON4bits.PENL = 1;    //PWMx module controls the PWMxL pin
    IOCON4bits.PMOD = 0b11; //PWMx I/O pin pair is in the True Independent Output mode
    PWMCON4bits.ITB = 1;    //PHASEx register provides the time base period for this PWMx generator
    PWMCON4bits.MTBS = 1;   //PWMx generator uses the secondary master time base (1/64)
    FCLCON4 = 0x0003;       //Fault input is disabled
}
void initPWM5(){       //50Hz for serovomotors
    PHASE5 = 43750;      //Period of PWM5H     Fosc / (F_PWM*PWM_Input_Clock_Prescaler) ex : 140Mhz / (20kHz * 1) = 7000 or wih Fosc = 138.24MHz -> 6912
    SPHASE5 = 43750;     //Period of PWM5L
    PDC5 = 0;           //Duty cycle of PWM5H
    SDC5 = 0;           //Duty cycle of PWM5L
    //DTR5 = 0;           //Dead Time
    //ALTDTR5 = 0;
    IOCON5bits.PENH = 1;    //PWMx module controls the PWMxH pin
    IOCON5bits.PENL = 1;    //PWMx module controls the PWMxL pin
    IOCON5bits.PMOD = 0b11; //PWMx I/O pin pair is in the True Independent Output mode
    PWMCON5bits.ITB = 1;    //PHASEx register provides the time base period for this PWMx generator
    PWMCON5bits.MTBS = 1;   //PWMx generator uses the secondary master time base for synchronization and as the clock source for the PWMx generation logic
    FCLCON5 = 0x0003;
}
void initPWM6(){           //50Hz for serovomotors
    PHASE6 = 43750;         //Period of PWM6H     Fosc / (F_PWM*PWM_Input_Clock_Prescaler) ex : 140Mhz / 64*(50Hz * 1) = 43750
    SPHASE6 = 43750;        //Period of PWM6L
    PDC6 = 0;               //Duty cycle of PWM6H
    SDC6 = 0;               //Duty cycle of PWM6L
    IOCON6bits.PENH = 1;    //PWMx module controls the PWMxH pin
    IOCON6bits.PENL = 1;    //PWMx module controls the PWMxL pin
    IOCON6bits.PMOD = 0b11; //PWMx I/O pin pair is in the True Independent Output mode
    PWMCON6bits.ITB = 1;    //PHASEx register provides the time base period for this PWMx generator
    PWMCON6bits.MTBS = 1;   //PWMx generator uses the secondary master time base for synchronization and as the clock source for the PWMx generation logic
    FCLCON6 = 0x0003;       //Fault input is disabled
}

void servoUs(unsigned char servo, double value){
    if(value < 0)
        value = 0;
    else if(value > 20000)   //Max
        value = 20000;
    value = value * 43750 / 20000;
    switch(servo){
        case SERVO0:
            SDC4 = (int)value;
            break;
        case SERVO1:
            PDC1 = (int)value;
            break;
        case SERVO2:
            SDC1 = (int)value;
            break;
        case SERVO3:
            SDC5 = (int)value;
            break;
        case SERVO4:
            PDC5 = (int)value;
            break;
        case SERVO5:
            SDC6 = (int)value;
            break;
        case SERVO6:
            PDC6 = (int)value;
            break;
        default :
            return;
    }
}
void testServo(){   //ATTENTION NE PAS BRANCHER TOUS LES SERVOS EN MEME TEMPS !
    double i = 1000;
    while(1){
        for(i = 0; i <= 5000; i++){
            servoUs(SERVO0,i);
            servoUs(SERVO1,i);
            servoUs(SERVO2,i);
            servoUs(SERVO3,i);
            servoUs(SERVO4,i);
            servoUs(SERVO5,i);
            servoUs(SERVO6,i);
            delay_ms(1);
        }
        for(i = 5000; i >= 0; i--){
            servoUs(SERVO0,i);
            servoUs(SERVO1,i);
            servoUs(SERVO2,i);
            servoUs(SERVO3,i);
            servoUs(SERVO4,i);
            servoUs(SERVO5,i);
            servoUs(SERVO6,i);
            delay_ms(1);
        }
    }
}
void testMotor(){
    double i = 1000;
    while(1){
        for(i = 3000; i <= 6000; i+=10){
            PDC3 = (int)i;
            SDC3 = (int)i;
            delay_ms(1);
        }
        for(i = 6000; i >= 3000; i-=10){
            PDC3 = (int)i;
            SDC3 = (int)i;
            delay_ms(1);
        }
    }
}
void sendToMotor(double valueR, double valueL){
    double vbat = readBattery();
    if(valueL > 0){
        valueL += DEAD_ZONE;
    }
    else if(valueL < 0){
        valueL -= DEAD_ZONE;
    }
    if(valueR > 0){
        valueR += DEAD_ZONE + COEF_MOT_BO;
    }
    else if(valueR < 0){
        valueR -= DEAD_ZONE - COEF_MOT_BO;
    }
    if(valueL <= -VSAT){
        SENS_L = BACKWARD;
        PWM_L = PWM_PR_L * (VSAT / vbat);
    }
    else if(valueL >= VSAT){
        SENS_L = FORWARD;
        PWM_L = PWM_PR_L * (VSAT / vbat);
    }
    else if(valueL < -DEAD_ZONE){
        SENS_L = BACKWARD;
        PWM_L = -(int)(valueL * PWM_PR_L / vbat);
    }
    else if(valueL > DEAD_ZONE){
        SENS_L = FORWARD;
        PWM_L = (int)(valueL * PWM_PR_L / vbat);
    }
    else{    //DEADZONE
        if(finalPoint == 1)
            PWM_L = 0;
    }
    
    if(valueR <= -VSAT){
        SENS_R = BACKWARD;
        PWM_R = PWM_PR_R * (VSAT / vbat);
    }
    else if(valueR >= VSAT){
        SENS_R = FORWARD;
        PWM_R = PWM_PR_R * (VSAT / vbat);
    }
    else if(valueR < -DEAD_ZONE){
        SENS_R = BACKWARD;
        PWM_R = -(int)(valueR * PWM_PR_R / vbat);
    }
    else if(valueR > DEAD_ZONE){
        SENS_R = FORWARD;
        PWM_R = (int)(valueR * PWM_PR_R / vbat);
    }
    else{    //DEADZONE
        if(finalPoint == 1)
            PWM_R = 0;
    }
}
void testSendToMotor(double valueR, double valueL){
    double vbat = readBattery();
    if(valueL <= -VSAT){
        SENS_L = BACKWARD;
        PWM_L = PWM_PR_L * (VSAT / vbat);
    }
    else if(valueL >= VSAT){
        SENS_L = FORWARD;
        PWM_L = PWM_PR_L * (VSAT / vbat);
    }
    else if(valueL < 0){
        SENS_L = BACKWARD;
        PWM_L = -(int)(valueL * PWM_PR_L / vbat);
    }
    else{
        SENS_L = FORWARD;
        PWM_L = (int)(valueL * PWM_PR_L / vbat);
    }
    
    if(valueR <= -VSAT){
        SENS_R = BACKWARD;
        PWM_R = PWM_PR_R * (VSAT / vbat);
    }
    else if(valueR >= VSAT){
        SENS_R = FORWARD;
        PWM_R = PWM_PR_R * (VSAT / vbat);
    }
    else if(valueR < 0){
        SENS_R = BACKWARD;
        PWM_R = -(int)(valueR * PWM_PR_R / vbat);
    }
    else{
        SENS_R = FORWARD;
        PWM_R = (int)(valueR * PWM_PR_R / vbat);
    }
}
void motorVoltage(uint8_t id, double voltage){
    double vbat = readBattery();
    
    switch(id){
        case ID_PUMP:{
            double PWM_VALUE = 0;
            if(voltage <= -VSAT_PUMP){
                SENS_PUMP = BACKWARD;
                PWM_VALUE = PWM_PR_PUMP * (VSAT_PUMP / vbat);
            }
            else if(voltage >= VSAT_PUMP){
                SENS_PUMP = FORWARD;
                PWM_VALUE = PWM_PR_PUMP * (VSAT_PUMP / vbat);
            }
            else if(voltage < 0){
                SENS_PUMP = BACKWARD;
                PWM_VALUE = -(int)(voltage * PWM_PR_PUMP / vbat);
            }
            else{
                SENS_PUMP = FORWARD;
                PWM_VALUE = (int)(voltage * PWM_PR_PUMP / vbat);
            }
            if(PWM_VALUE > PWM_PR_MOTOR_LINEAR){
                PWM_VALUE = PWM_PR_MOTOR_LINEAR;
            }
            else if(PWM_VALUE < 0){
                PWM_VALUE = 0;
            }
            PWM_PUMP = (uint16_t)PWM_VALUE;
            break;
        }
        case ID_MOTOR_LINEAR:{
            double PWM_VALUE = 0;
            if(voltage <= -VSAT_MOTOR_LINEAR){
                SENS_MOTOR_LINEAR = BACKWARD;
                PWM_VALUE = PWM_PR_MOTOR_LINEAR * (VSAT_MOTOR_LINEAR / vbat);
            }
            else if(voltage >= VSAT_MOTOR_LINEAR){
                SENS_MOTOR_LINEAR = FORWARD;
                PWM_VALUE = PWM_PR_MOTOR_LINEAR * (VSAT_MOTOR_LINEAR / vbat);
            }
            else if(voltage < 0){
                SENS_MOTOR_LINEAR = BACKWARD;
                PWM_VALUE = -(int)(voltage * PWM_PR_MOTOR_LINEAR / vbat);
            }
            else{
                SENS_MOTOR_LINEAR = FORWARD;
                PWM_VALUE = (int)(voltage * PWM_PR_MOTOR_LINEAR / vbat);
            }
            if(PWM_VALUE > PWM_PR_MOTOR_LINEAR){
                PWM_VALUE = PWM_PR_MOTOR_LINEAR;
            }
            else if(PWM_VALUE < 0){
                PWM_VALUE = 0;
            }
            PWM_MOTOR_LINEAR = (uint16_t)PWM_VALUE;
            break;
        }
        /*case ID_MOTOR_LEFT:
            
            break;
        case ID_MOTOR_RIGHT:
            
            break;*/
        default:
            break;  
    }
}