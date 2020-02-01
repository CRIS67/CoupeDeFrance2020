#ifndef HMI_H
#define HMI_H

#include <iostream>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#include "SPI.hpp"
#include "robot.hpp"

#define ERASE_TIME 			100
#define EraseTime 			100

#define JAUNE               1
#define BLEU                2

void* thread_Hmi(void *threadid);

class Hmi : public Robot {
	public:
		Hmi(std::string nom, SPI *pSpi, uint8_t id);
		virtual ~Hmi();
		void DecodMsg(uint8_t buf[]);
		bool startThreadDetection();

		uint8_t CoteChoisi();
		bool isStopPrgm();
		bool isStopMain();
		void SetPos(int in, int pos);
		void EraseScreen(int in_era);
		void SetTxt(int in, std::string txt);
		void SetTxtFull(std::string txt_in);
		void setScore(int score);
	protected:
		bool m_stopPrgm = false;
		bool m_stopMain = false;
		uint8_t m_hmi_cote = 0;
};


#endif // HMI_H