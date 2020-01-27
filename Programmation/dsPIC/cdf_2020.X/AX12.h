
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "constant.h"

void initAX12();
void push(char c);
char pop();
void writeDataAX12(char id, char length, char* datas);
void turnLedAX12(char id, char state);
void blockAnswerAX12(char id);
void rotateToAX12(char id, char posL, char posH);
void testBlinkOn();
void testBlinkOff();

#endif	/* XC_HEADER_TEMPLATE_H */

