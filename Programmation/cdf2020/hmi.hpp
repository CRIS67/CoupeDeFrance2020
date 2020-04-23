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

#define HMI_CMD_POS		6
#define HMI_CMD_TXT		8
#define HMI_CMD_SCORE	13
#define HMI_CMD_RST_UC	14

#define HMI_RET_COTE	15
#define HMI_RET_OFF_PI	17

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
		void sendSPI(uint8_t *buf, uint8_t bufSize);

		bool isStopPrgm(void);
		bool isStopMain(void);

		void resetPic(void);

		bool startThreadDetection();
		void stopThreadDetection();
		bool isContinueThread();
		uint8_t CoteChoisi(void);

    	uint8_t bufferRx[SIZE_BUFFER_RX];
		uint8_t iRxIn = 0;
		uint8_t iRxOut = 0;
		uint8_t nbMsgReceived = 0;
		bool receivingMsg = false;
		uint8_t currentMsgSize = 0;
		uint8_t nbBytesReceived = 0;
		uint32_t nbBytesReceivedTotal = 0;
    protected:
		std::mutex m_mutex;
		uint8_t m_id;	//id of this SPI slave
		pthread_t m_thread;
		bool m_continueThread;
		bool m_fillBuffer = false;
		bool m_stopPrgm = false;
		bool m_stopMain = false;
		uint8_t m_hmi_cote = 0;
		SPI *m_pSpi;	//pointer to SPI instance
    private:
};

#endif // HMI_H
