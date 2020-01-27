/*
 * File:   QEI.c
 * Author: Quentin BOYER
 *
 * Created on 26 octobre 2017, 22:03
 * 
 * Notes:
 *  QEI : Quadrature Encoder Interface
 *         
 */

#include "QEI.h"

void initQEI() {
    initQEI1();
    initQEI2();
}

void initQEI1(){
    QEI1CONbits.CCM = 0b00;     //Quadrature Encoder Interface (x4 mode) Count mode is selected
    QEI1CONbits.GATEN = 0;      //External gate signal does not affect position counter/timer operation
    QEI1CONbits.CNTPOL = 1;     //Counter direction is NEGATIVE unless modified by external up/down signal
    QEI1CONbits.INTDIV = 0b000; //1:1 prescale value
    QEI1CONbits.PIMOD = 0b000;  //Index input event does not affect position counter
    QEI1CONbits.QEIEN = 1;      //Module counters are enabled
    
    //QEI1IOC = 0;
    QEI1IOCbits.FLTREN = 1;     //Input pin digital filter is enabled
    
    QEI1STAT = 0;
    
}

void initQEI2(){
    QEI2CONbits.CCM = 0b00;     //Quadrature Encoder Interface (x4 mode) Count mode is selected
    QEI2CONbits.GATEN = 0;      //External gate signal does not affect position counter/timer operation
    QEI2CONbits.CNTPOL = 0;     //Counter direction is positive unless modified by external up/down signal
    QEI2CONbits.INTDIV = 0b000; //1:1 prescale value
    QEI2CONbits.PIMOD = 0b000;  //Index input event does not affect position counter
    QEI2CONbits.QEIEN = 1;      //Module counters are enabled
    
    //QEI1IOC = 0;
    QEI2IOCbits.FLTREN = 1;     //Input pin digital filter is enabled
    
    QEI2STAT = 0;
}