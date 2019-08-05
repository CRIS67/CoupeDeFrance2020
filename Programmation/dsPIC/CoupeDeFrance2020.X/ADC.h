/* 
 * File: ADC.h  
 * Author: Quentin BOYER
 * Comments: 
 * Revision history: 1
 */

#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>
#include "constant.h"

void initADC();
void initADC1();
int readADC1();
int readADC(uint16_t channel);
double readBattery();


#endif	/* ADC_H */