/* 
 * File: ADC.h  
 * Author: Quentin BOYER
 * Comments: 
 * Revision history: 1
 */

#ifndef DMA_H
#define	DMA_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <string.h>
#include <stdint.h>
#include "constant.h"

void initDMA();
void initDMA0();
void initDMA1();
void sendDMA(char* buffer, unsigned int size);


#endif	/* DMA_H */