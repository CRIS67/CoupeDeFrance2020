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
#include <pthread.h>

#include "SPI.hpp"

#define DEBUG_HMI_ENABLE_PRINT			1

#if(DEBUG_HMI_ENABLE_PRINT)
#define DEBUG_HMI_PRINT(x) 				std::cout << "Hmi Debug> " << x << std::endl;
#else
#define DEBUG_HMI_PRINT(x)				
#endif

#define ERASE_TIME 100
#define EraseTime 100

#define SIZE_BUFFER_RX	1000

#define HMI_SHUTDOWN_PI 17

void* thread_HMI(void *threadid);

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
		void checkMessages();
		void sendReceiveSPI(uint8_t data);
		void setScore(int score);

		//void setTurnOffPi();
		//void setStopMain();

		void resetPic(void);

		bool startThreadDetection();
		void stopThreadDetection();
		bool isContinueThread();

    protected:
    	uint8_t bufferRx[SIZE_BUFFER_RX];
		uint8_t iRxIn = 0;
		uint8_t iRxOut = 0;
		uint8_t nbMsgReceived = 0;
		bool receivingMsg = false;
		uint8_t currentMsgSize = 0;
		uint8_t nbBytesReceived = 0;
		uint32_t nbBytesReceivedTotal = 0;

		std::mutex m_mutex;
		pthread_t m_thread;
		bool m_continueThread;
		bool m_fillBuffer = false;
    private:
		uint8_t m_id;	//id of this SPI slave
		SPI *m_pSpi;	//pointer to SPI instance
};

#endif // HMI_H
