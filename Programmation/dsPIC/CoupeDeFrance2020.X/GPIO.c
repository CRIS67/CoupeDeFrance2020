/*
 * File:   GPIO.c
 * Author: Quentin BOYER
 *
 * Created on September 18, 2017, 11:23 PM
 */

#include <p33EP512GM310.h>

#include "GPIO.h"
#include "interrupt.h"

void initGPIO() {
    
    //1 -> input / 0 -> output
    /*
    TRISA = 0xFFFF;
    TRISB = 0xFFFF;
    TRISC = 0xFFFF;
    TRISD = 0xFFFF;
    TRISE = 0xFFFF;
    TRISF = 0xFFFF;
    TRISG = 0xFFFF;
    */
    
    TRISAbits.TRISA0 = 1;   //RUPT_ACT_4
    TRISAbits.TRISA1 = 1;   //RUPT_ACT_3
    TRISAbits.TRISA7 = 0;   //PWM_SERVO_0
    TRISAbits.TRISA8 = 1;   //ECHO US0
    TRISAbits.TRISA10 = 0;  //PWM_MOT_CANON
    TRISAbits.TRISA11 = 1;  //RUPT_ACT_5
    TRISAbits.TRISA12 = 1;  //RUPT_ASS_2
    
    TRISBbits.TRISB0 = 1;   //RUPT_ACT_2
    TRISBbits.TRISB1 = 1;   //RUPT_ACT_1
    TRISBbits.TRISB4 = 0;   //TRIG_US_0
    TRISBbits.TRISB5 = 1;   //RX_BT
    TRISBbits.TRISB6 = 0;   //TX_BT
    TRISBbits.TRISB8 = 1;   //COURANT_ASS_0
    TRISBbits.TRISB9 = 1;   //COURANT_ASS_1
    TRISBbits.TRISB10 = 0;  //PWM_ASS_0
    TRISBbits.TRISB11 = 0;  //PWM_ASS_1
    TRISBbits.TRISB12 = 0;  //PWM_ACT_1
    TRISBbits.TRISB13 = 0;  //PWM_ACT_0
    TRISBbits.TRISB14 = 0;  //PWM_SERVO_1
    TRISBbits.TRISB15 = 0;  //PWM_SERVO_2
    
    TRISCbits.TRISC0 = 1;   //US_NUM_1
    TRISCbits.TRISC1 = 1;   //US_NUM_0
    TRISCbits.TRISC4 = 1;   //MES_BAT
    TRISCbits.TRISC8 = 0;   //SPI_SS_MOD1
    TRISCbits.TRISC9 = 0;   //SPI_CLOCK
    TRISCbits.TRISC11 = 1;  //ECHO_US_5
    
    TRISDbits.TRISD1 = 0;   //PWM_SERVO_3
    TRISDbits.TRISD2 = 0;   //PWM_SERVO_4
    TRISDbits.TRISD3 = 0;   //PWM_SERVO_5
    TRISDbits.TRISD4 = 0;   //PWM_SERVO_6
    TRISDbits.TRISD5 = 0;   //SPI_SS_MOD0
    TRISDbits.TRISD6 = 0;   //SPI_MOSI
    //TRISDbits.TRISD12 = 0;  //TRIG_US_4
    TRISDbits.TRISD13 = 1;  //SPI_MISO
    TRISDbits.TRISD14 = 1;  //ECHO_US_1
    TRISDbits.TRISD15 = 0;  //TRIG_US_1
    
    TRISEbits.TRISE0 = 1;   //RX_RPI
    TRISEbits.TRISE1 = 0;   //TX_RPI
    TRISEbits.TRISE8 = 1;   //RUPT_ASS_0
    TRISEbits.TRISE9 = 1;   //RUPT_ASS_3
    TRISEbits.TRISE12 = 1;  //ECHO_US_3
    TRISEbits.TRISE13 = 0;  //TRIG_US_3
    TRISEbits.TRISE14 = 1;  //ECHO_US_2
    TRISEbits.TRISE15 = 0;  //TRIG_US_2
 
    TRISFbits.TRISF0 = 0;   //ACT_POMPE
    TRISFbits.TRISF1 = 0;   //ACT_RAB
    TRISFbits.TRISF4 = 0;   //LED_1
    TRISFbits.TRISF5 = 0;   //LED_2
    TRISFbits.TRISF9 = 1;   //RUPT_ACT_0
    TRISFbits.TRISF10 = 1;  //SIGNAL_JUMPER
    TRISFbits.TRISF12 = 0;  //TRIG_US_4
    TRISFbits.TRISF13 = 1;  //ECHO_US_4
    
    TRISGbits.TRISG0 = 0;   //SENS_ASS_0
    TRISGbits.TRISG1 = 0;   //SENS_ASS_1
    TRISGbits.TRISG3 = 0;   //LED_0
    TRISGbits.TRISG6 = 1;   //CODEUR_ASS_0A
    TRISGbits.TRISG7 = 1;   //CODEUR_ASS_0B
    TRISGbits.TRISG8 = 1;   //CODEUR_ASS_1A
    TRISGbits.TRISG9 = 1;   //CODEUR_ASS_1B
    TRISGbits.TRISG10 = 1;  //RUPT_ASS_1
    TRISGbits.TRISG11 = 0;  //TRIG_US_5
    TRISGbits.TRISG12 = 0;   //SENS_ACT_0
    TRISGbits.TRISG13 = 0;   //SENS_ACT_1
    TRISGbits.TRISG15 = 0;  //DATA_AX12
    
    
    ANSELA = 0x0000;
    ANSELB = 0x0000;
    ANSELC = 0x0000;
    ANSELD = 0x0000;
    ANSELE = 0x0000;
    ANSELF = 0x0000;
    ANSELG = 0x0000;
     
    ANSELCbits.ANSC4 = 1;   //MESURE_BAT
    ANSELBbits.ANSB8 = 1;   //COURANT_ASS_ 0
    ANSELBbits.ANSB9 = 1;   //COURANT_ASS_ 1

    //1 -> open-drain
    ODCA = 0x0000;
    ODCB = 0x0000;
    ODCC = 0x0000;
    ODCD = 0x0000;
    ODCE = 0x0000;
    ODCF = 0x0000;
    ODCG = 0x0000;
    //1 -> pull-up resistor
    CNPUA = 0x0000;
    CNPUB = 0x0000;
    CNPUC = 0x0000;
    CNPUD = 0x0000;
    CNPUE = 0x0000;
    CNPUF = 0x0000;
    CNPUG = 0x0000;
    
    /*CNPUAbits.CNPUA8 = 1;   //ECHO_0
    CNPUDbits.CNPUD14 = 1;  //ECHO_1
    CNPUEbits.CNPUE14 = 1;  //ECHO_2
    CNPUEbits.CNPUE12 = 1;  //ECHO_3
    CNPUFbits.CNPUF13 = 1;  //ECHO_4
    CNPUCbits.CNPUC11 = 1;  //ECHO_5*/
    
    //1 -> pull-down resistor
    CNPDA = 0x0000;
    CNPDB = 0x0000;
    CNPDC = 0x0000;
    CNPDD = 0x0000;
    CNPDE = 0x0000;
    CNPDF = 0x0000;
    CNPDG = 0x0000;
    
    CNPDAbits.CNPDA8 = 1;   //ECHO_0
    CNPDDbits.CNPDD14 = 1;  //ECHO_1
    CNPDEbits.CNPDE14 = 1;  //ECHO_2
    CNPDEbits.CNPDE12 = 1;  //ECHO_3
    CNPDFbits.CNPDF13 = 1;  //ECHO_4
    CNPDCbits.CNPDC11 = 1;  //ECHO_5
    
    //1 -> genere an interrupt request on a Change-of-State
    CNENA = 0x0000;
    CNENB = 0x0000;
    CNENC = 0x0000;
    CNEND = 0x0000;
    CNENE = 0x0000;
    CNENF = 0x0000;
    CNENG = 0x0000;
    CNENAbits.CNIEA8 = 1;
    CNENDbits.CNIED14 = 1;
    CNENEbits.CNIEE14 = 1;
    CNENEbits.CNIEE12 = 1;
    CNENFbits.CNIEF13 = 1;
    CNENCbits.CNIEC11 = 1;
    //CNENGbits.CNIEG12 = 1;
    //CNENGbits.CNIEG13 = 1;
    //1 -> High(3.3V) or 0V(open-drain) / 0 -> Low(0V) or high-impedance(open-drain)
    LATA = 0x0000;
    LATB = 0x0000;
    LATC = 0x0000;
    LATD = 0x0000;
    LATE = 0x0000;
    LATF = 0x0000;
    LATG = 0x0000;
    
    /*Peripheral Pin Select (PPS)*/
    //QEI
	RPINR14bits.QEA1R = 0b1110110;  //QEI1A -> RPI118 (pin10)
    RPINR14bits.QEB1R = 0b1110111;  //QEI1B -> RPI119 (pin11)
    
    RPINR16bits.QEA2R = 0b1111000;  //QEI2A -> RPI120 (pin12)
    RPINR16bits.QEB2R = 0b1111001;  //QEI2B -> RPI121 (pin14)
}