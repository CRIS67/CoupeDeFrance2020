/*
 * File:   actuators.c
 * Author: Quentin BOYER
 *
 * Created on 27 juin 2019, 23:53
 */


#include "actuators.h"

void setMotLin(uint8_t state){
    /*double acc = 5;     //v/s^1
    double speed = 15;   //v
    double dt = 0.01;   //s
    
    double voltage = 0;
    
    unsigned long t1 = millis();
    if(state == 0){ //out
        while(!RUPT_ACT_0){ //wait for rupt
            if( (millis() - t1) > (dt*1000) ){
                if(voltage < speed){
                    voltage += acc*dt;
                    if(voltage > speed){
                        voltage = speed;
                    }
                    motorVoltage(ID_MOTOR_LINEAR,voltage);
                }
            }
        }
        motorVoltage(ID_MOTOR_LINEAR,-voltage); //brake
        delay_ms(80);
        motorVoltage(ID_MOTOR_LINEAR,0);    //stop
    }
    else if(state == 1){ //in
        uint8_t nbCurrent = 0;
        while(!RUPT_ACT_1 && nbCurrent < 10){ //wait for rupt
            if( (millis() - t1) > (dt*1000) ){
                if(voltage > -speed){
                voltage -= acc*dt;
                if(voltage < -speed){
                    voltage = -speed;
                }
                motorVoltage(ID_MOTOR_LINEAR,voltage);
                }
            }
            double current = readAdcLowPass(ADC_CHANNEL_I_PUMP,100,0.01);
            if(current > 30 && current < 40)
                nbCurrent++;
            else
                nbCurrent = 0;
            plot(1,(uint32_t)((int32_t)(current)));
        }
        motorVoltage(ID_MOTOR_LINEAR,-voltage); //brake
        delay_ms(80);
        motorVoltage(ID_MOTOR_LINEAR,0);    //stop
    }*/
    
}
void setMotLinSpeed(uint8_t state, uint8_t arg_speed, uint32_t timeout){
    /*double acc = 5;     //v/s^1
    double speed = 12.0*arg_speed/255.0;   //v
    double dt = 0.01;   //s
    
    double voltage = 0;
    
    unsigned long t1 = millis();
    if(state == 0){ //out
        while(!RUPT_ACT_0){ //wait for rupt
            if( (millis() - t1) > (dt*1000) ){
                t1 = millis();
                if(voltage < speed){
                    voltage += acc*dt;
                    if(voltage > speed){
                        voltage = speed;
                    }
                    motorVoltage(ID_MOTOR_LINEAR,voltage);
                }
            }
        }
        motorVoltage(ID_MOTOR_LINEAR,-voltage); //brake
        delay_ms(80);
        motorVoltage(ID_MOTOR_LINEAR,0);    //stop
    }
    else if(state == 1){ //in
        uint8_t nbCurrent = 0;
        while(!RUPT_ACT_1 && nbCurrent < 10){ //wait for rupt
            if( (millis() - t1) > (dt*1000) ){
                t1 = millis();
                if(voltage > -speed){
                voltage -= acc*dt;
                if(voltage < -speed){
                    voltage = -speed;
                }
                motorVoltage(ID_MOTOR_LINEAR,voltage);
                }
            }
            double current = readAdcLowPass(ADC_CHANNEL_I_PUMP,100,0.01);
            if(current > 30 && current < 40)
                nbCurrent++;
            else
                nbCurrent = 0;
            plot(1,(uint32_t)((int32_t)(current)));
        }
        motorVoltage(ID_MOTOR_LINEAR,-voltage); //brake
        delay_ms(80);
        motorVoltage(ID_MOTOR_LINEAR,0);    //stop
    }*/
}
double readAdcLowPass(uint8_t channel, uint16_t nbSamples, double coefLP){
	double oldCurrent = 0;
    double current;
	uint16_t cnt;
	for(cnt = 0; cnt < nbSamples;cnt++){
		int mes = readADC(channel);
		current = mes;
		current = oldCurrent + (current - oldCurrent)*coefLP;
		oldCurrent = current;
	}
	return current;
}
double readAdcMean(uint8_t channel, uint16_t nbSamples){
	double tot = 0;
	uint16_t cnt;
	for(cnt = 0; cnt < nbSamples;cnt++){
		int mes = readADC(channel);
        tot += mes;
	}
	return tot / nbSamples;
}
