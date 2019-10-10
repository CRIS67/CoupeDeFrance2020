#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>

#include "SPI.hpp"

#define SIZE_BUFFER_RX	1000

#define NB_FLUSH_MIN	4
#define TEMP_MAX_FLUSH	1000

#define ACT_CMD_PUMP		1
#define ACT_CMD_SERVO_EXT	4
#define ACT_CMD_SERVO 		7
#define ACT_CMD_COLOR		10
#define ACT_CMD_CUR 		13
#define ACT_CMD_CUR_FULL	16
#define ACT_CMD_COLOR_FULL	19
#define ACT_CMD_SEUIL_LEVEL	22
#define ACT_CMD_SEUIL_RST	29
#define ACT_CMD_RST_UC		30

#define ACT_RET_COLOR_1		 10
#define ACT_RET_COLOR_2		 11
#define ACT_RET_COLOR_3		 12
#define ACT_RET_CUR_1		 13
#define ACT_RET_CUR_2		 14
#define ACT_RET_CUR_3		 15
#define ACT_RET_CUR_FULL_1	 16
#define ACT_RET_CUR_FULL_2	 17
#define ACT_RET_CUR_FULL_3	 18
#define ACT_RET_COLOR_FULL_1 19
#define ACT_RET_COLOR_FULL_2 20
#define ACT_RET_COLOR_FULL_3 21

#define SERVO_VALUE_HIGH	700
#define SERVO_VALUE_MIDDLE	1000
#define SERVO_VALUE_DROP	1500
#define SERVO_VALUE_LOW		1600

void* thread_Actuators(void *threadid);

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
		//void Launchtest();
		//int debugGetCurrent(int nb_bras);
		//int debugGetCurrentFull(int nb_bras);
		//int debugGetColor(int nb_bras);
		
		void ChangeColorLevel(int in, int val);
		void GetColorFull(int in);
		void ResetColorDefault(void);
		void ResetAtmega(void);

		void checkMessages();
		void sendSPI(uint8_t *buf, uint8_t bufSize);
		void sendReceiveSPI(uint8_t data);
		void flush(uint16_t nbBytes);
		bool flushDyn(void);


		bool startThreadDetection();
		void stopThreadDetection();
		bool isContinueThread();
		
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
		SPI *m_pSpi;	//pointer to SPI instance
    private:
};

#endif // ACTUATORS_H
