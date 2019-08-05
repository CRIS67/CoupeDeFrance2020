/* 
 * File: interrupt.h  
 * Author: Quentin BOYER
 * Comments: check most appropriate variable type <-> speed of execution 
 *              optimization : remove prevProcessVariable
 * Revision history: 1
 */

#ifndef INTERRUPT_H
#define	INTERRUPT_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "constant.h"
#define _ISR_PSV        __attribute__((__interrupt__, __auto_psv__))
typedef enum pinCN pinCN;
enum pinCN
{
    DEFAULT,RUPT_1, RUPT_2, ECHO, ECHO_0, ECHO_1, ECHO_2, ECHO_3, ECHO_4, ECHO_5
};
void initInt();


#endif	/* INTERRUPT_H */