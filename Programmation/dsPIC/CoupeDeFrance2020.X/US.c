/*
 * File:   US.c
 * Author: Quentin BOYER
 *
 * Created on 12 avril 2018, 17:30
 * 
 */

#include "US.h"

//volatile char   n;              //index of current ultrasonic sensor
volatile char   US_ON[NB_US];    //
volatile char   US_R[NB_US];    //
volatile double US[NB_US];      //measured distance

void initUS(){
    int i;
    for(i = 0; i < NB_US; i++){
        US[i] = 0;
        US_ON[i] = 0;
        US_R[i] = 0;
    }
}

