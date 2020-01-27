/*
 * File:   clock.c
 * Author: Quentin BOYER
 *
 * Created on September 18, 2017, 10:17 PM
 */

#include "clock.h"

void initClock(){
    PLLFBDbits.PLLDIV = CLOCK_M - 2;                //PLLDIV = M - 2 
    CLKDIVbits.DOZEN = 0;                           //Processor clock and peripheral clock ratio are forced to 1:1
    CLKDIVbits.PLLPRE = CLOCK_N1 - 2;               //N1 = Input divided by 4
    CLKDIVbits.PLLPOST = (CLOCK_N2/2) - 1;          //N2 = Output divided by 2
    __builtin_write_OSCCONH(0b11);                  //OSCCONbits.NOSC = 0b011; Primary Oscillator with PLL (XTPLL, HSPLL, ECPLL)
    __builtin_write_OSCCONL(OSCCON | 0x01);         //request clock switching
    while (OSCCONbits.COSC!= 0b011);                //wait for clock switch
    while(!OSCCONbits.LOCK);                        //wait for PLL LOCK
}
