#ifndef HMI_H
#define HMI_H

#include <iostream>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "SPI.hpp"

#define ERASE_TIME 100
#define EraseTime 100

class HMI
{
    public:
        HMI(SPI *pSpi,uint8_t id);
        virtual ~HMI();

        void SendManual(void);
		int Send(int in);
		void SetPrgm(int in, std::string txt);
		void SetPos(int in, int pos);
		void SetTxt(int in, std::string txt);
		void SetTxtFull(std::string txt_in);
		void SetPosXY(int pos_x, int pos_y);
		void EraseScreen(int in_era);
		void flush(uint16_t nb);
		void setScore(int score);
		void resetPic(void);

    protected:
    private:
		uint8_t m_id;	//id of this SPI slave
		SPI *m_pSpi;	//pointer to SPI instance
};

#endif // HMI_H
