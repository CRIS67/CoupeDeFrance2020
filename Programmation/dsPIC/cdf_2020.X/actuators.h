/*
 * File:   actuators.h
 * Author: Quentin BOYER
 *
 * Created on 27 juin 2019, 23:53
 */

#ifndef ACTUATORS_H
#define	ACTUATORS_H

#include <xc.h>
#include <stdint.h>
#include "constant.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"

void setMotLin(uint8_t state);
void setMotLinSpeed(uint8_t state, uint8_t arg_speed, uint32_t timeout);
double readAdcLowPass(uint8_t channel, uint16_t nbSamples, double coefLP);
double readAdcMean(uint8_t channel, uint16_t nbSamples);

#endif	/* ACTUATORS_H */