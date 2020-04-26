#ifndef LIDAR_H
#define LIDAR_H

#define DEBUG_LIDAR_ENABLE_PRINT			0

#if(DEBUG_LIDAR_ENABLE_PRINT)
#define DEBUG_LIDAR_PRINT(x) 				std::cout << "Lidar Debug> " << x << std::endl;
#else
#define DEBUG_LIDAR_PRINT(x)				
#endif

#define SIZE_BUFFER_RX	1000

#define CODE_VAR_DISTANCE   1
#define CODE_VAR_ANGLE      2

#define LIDAR_CMD_DEBUG               1
#define LIDAR_CMD_START               2
#define LIDAR_CMD_STOP                3
#define LIDAR_CMD_GET_DISTANCE        4
#define LIDAR_CMD_GET_ANGLE           5
#define LIDAR_CMD_GET_STARTBIT        6
#define LIDAR_CMD_GET_QUALITY         7
#define LIDAR_CMD_SET_AUTOSTART       8
#define LIDAR_CMD_RESET_AUTOSTART     9

#define LIDAR_CMD_GET_DATA_AVAILABLE    10
#define LIDAR_CMD_GET_DETECTED_POINTS   11
#define LIDAR_CMD_GET_RAW_POINT         12

#define LIDAR_CMD_SET_SPEED             20
#define LIDAR_CMD_GET_SPEED             21


#define LIDAR_RET_DEBUG_DEBUG       42 
#define LIDAR_RET_DEBUG_START       43 
#define LIDAR_RET_DEBUG_STOP        44
#define LIDAR_RET_DATA_AVAILABLE    100
#define LIDAR_RET_DETECTED_POINTS   101
#define LIDAR_RET_RAW_POINT         102

#define LIDAR_RET_SPEED             121

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include <iostream>
#include <pthread.h>

#include "SPI.hpp"
#include "web.hpp"



void* thread_Lidar(void *threadid);

class Lidar
{
    public:
        Lidar(SPI *pSpi,uint8_t id, Web *pWeb);
        virtual ~Lidar();
		
		void start();
		void stop();
		
		void sendReceiveSPI(uint8_t data);
		void flush(uint16_t nbBytes);
		void checkMessages();
		void sendSPI(uint8_t *buf, uint8_t bufSize);
		void getAvailableData();
		void getRawPoint();
		void sendGetDetectedPoints();
		void setSpeed(uint8_t speed);
		void getSpeed();
		
		bool startThreadDetection();
		void stopThreadDetection();
		
		bool isContinueThread();
		
		void addDetectedPoint(pointFloat2d p);
		std::queue<pointFloat2d> getDetectedPoints();
		std::queue<pointFloat2d> getAndClearDetectedPoints();
		
		void setFillBuffer(bool b);
		bool getFillBuffer();
		
		int16_t x = 1500,y = 1000, t = 45;
		uint8_t bufferRx[SIZE_BUFFER_RX];
		uint8_t iRxIn = 0;
		uint8_t iRxOut = 0;
		bool receivingMsg = false;
		uint8_t currentMsgSize = 0;
		uint8_t nbBytesReceived = 0;
		uint32_t nbBytesReceivedTotal = 0;
		uint8_t nbMsgReceived = 0;
    protected:
		std::mutex m_mutex;
		uint8_t m_id;			//id of this SPI slave
		pthread_t m_thread;
		bool m_continueThread;
		//int fd;
		SPI *m_pSpi;	//pointer to SPI instance
		Web *m_pWeb;	//pointer to SPI instance
		std::queue<pointFloat2d> m_qDetectedPoints;
		bool m_fillBuffer = false;
    private:
};

#endif // LIDAR_H
