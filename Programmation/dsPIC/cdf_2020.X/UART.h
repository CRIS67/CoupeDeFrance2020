#ifndef UART_H
#define	UART_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <string.h>
#include "constant.h"
#include "PID.h"
#include "AX12.h"

void initUART();
void initUART1();
void initUART2();
char pop();
char pop2();
void push(char c);
void push2(char c);
void plot(uint8_t id,uint32_t value);
char *itoa(int value);
char *dtoa(double value);
void CheckMessages();
void send(uint8_t *str,uint16_t size);
void sendPos();
void sendRupt();
void sendUS();
void sendLog(char *str);
void sendVar8(uint8_t varCode, uint8_t var);
void sendVar16(uint8_t varCode, uint16_t var);
void sendVar32(uint8_t varCode, uint32_t var);
void sendDouble(uint8_t varCode, double *ptrVar);
void sendLongDouble(uint8_t varCode, long double ptrVar);
void sendPosLongDouble();

void sendAllPID();
#define GET     1
#define SET     2

#define _TMR1   1
#define _TMR2   2

#endif	/* UART_H */
