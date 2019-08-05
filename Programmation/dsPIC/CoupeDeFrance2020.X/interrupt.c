/*
 * File:   interrupt.c
 * Author: Quentin BOYER
 *
 * Created on 28 octobre 2017, 16:51
 */

#include <p33EP512GM310.h>

#include "interrupt.h"

extern volatile char   US_ON[NB_US];
extern volatile char   US_R[NB_US];
extern volatile double US[NB_US];

volatile int i = 0;
volatile unsigned int saveA;
volatile unsigned int saveB;
volatile unsigned int saveC;
volatile unsigned int saveD;
volatile unsigned int saveE;
volatile unsigned int saveF;
volatile unsigned int saveG;

extern volatile unsigned char US_received;

void initInt() {
    INTCON1 = 0x8000;           //Disable traps and interrupt nesting
    INTCON2 = 0;                //Disable interruptions and traps and internal interrupt occurs on positive edge
    saveA = PORTA;
    saveB = PORTB;
    saveC = PORTC;
    saveD = PORTD;
    saveE = PORTE;
    saveF = PORTF;
    saveG = PORTG;
    IEC1bits.CNIE = 1;          //Enable Input Change Interrupt
    INTCON2bits.GIE = 1;        //Enable Global Interrupt
}
void _ISR_PSV _CNInterrupt(){   //Change Notification interrupt
    //print("CN_INT\n");
    IFS1bits.CNIF = 0;          // Clear CN interrupt
    pinCN p = DEFAULT;
    unsigned char n;
    
    if((saveA & (1 << 8)) != PORTAbits.RA8){
        p = ECHO;
        n = 0;
    }
    else if((saveD & (1 << 14)) != PORTDbits.RD14){
        p = ECHO;
        n = 1;
    }
    else if((saveE & (1 << 14)) != PORTEbits.RE14){
        p = ECHO;
        n = 2;
    }
    else if((saveE & (1 << 12)) != PORTEbits.RE12){
        p = ECHO;
        n = 3;
    }
    else if((saveF & (1 << 13)) != PORTFbits.RF13){
        p = ECHO;
        n = 4;
    }
    else if((saveC & (1 << 11)) != PORTCbits.RC11){
        p = ECHO;
        n = 5;
    }
    else if((saveG & (1 << 12)) != PORTGbits.RG12){
        p = RUPT_1;
    }
    else if((saveG & (1 << 13)) != PORTGbits.RG13){
        p = RUPT_2;
    }
    switch(p){
        case RUPT_1:
            //led = 1;
            break;
        case RUPT_2:
            //led = 0;
            break;
        case ECHO:
            if(US_R[n]){
                US[n] = TMR4;
                //T4CONbits.TON = 0;      //enable Timer4
                US_R[n] = 0;
                
                US_received = 1;
            }
            if(US_ON[n]){
                TMR4 = 0;
                //T4CONbits.TON = 1;      //enable Timer4
                US_ON[n] = 0;
                US_R[n] = 1;
            }
            break;
        default:
            break;
    }
    saveA = PORTA;
    saveB = PORTB;
    saveC = PORTC;
    saveD = PORTD;
    saveE = PORTE;
    saveF = PORTF;
    saveG = PORTG;
}