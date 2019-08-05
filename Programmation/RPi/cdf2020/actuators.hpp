#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

#include "SPI.hpp"

#define SERVO_VALUE_HIGH	700
#define SERVO_VALUE_MIDDLE	1000
#define SERVO_VALUE_DROP	1500
#define SERVO_VALUE_LOW		1600

class Actuators
{
    public:
        Actuators(SPI *pSpi,uint8_t id);
        virtual ~Actuators();
		void MoveServoExtr(int nb_bras, int pos);
		void MoveServo(int nb_bras, int pos);
		void SetPump(int nb_bras, int state);
		void GetColor(int nb_bras);
		void GetCurrent(int nb_bras);
		void GetCurrentFull(int nb_bras);
		/*void SetPrgm(void);
		void SetPos(void);
		void SetTxt(void);*/
		void Launchtest();
		void flush(uint16_t nb);
		int debugGetCurrent(int nb_bras);
		int debugGetCurrentFull(int nb_bras);
		int debugGetColor(int nb_bras);
		
		void ChangeColorLevel(int in, int val);
		uint16_t GetColorFull(int in);
		void ResetColorDefault(void);
		void ResetAtmega(void);
		
    protected:
    private:
		uint8_t m_id;	//id of this SPI slave
		//int m_fd;		//SPI file descriptor
		SPI *m_pSpi;	//pointer to SPI instance
};

#endif // ACTUATORS_H
