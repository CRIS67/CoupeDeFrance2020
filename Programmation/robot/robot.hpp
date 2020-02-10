#ifndef ROBOT_H
#define ROBOT_H

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#include "SPI.hpp"

#define DEBUG_ROBOT_ENABLE_PRINT		1

#if(DEBUG_ROBOT_ENABLE_PRINT)
#define DEBUG_ROBOT_PRINTLN(x) 				std::cout << m_nom << " > " << x << std::endl;
#define DEBUG_ROBOT_PRINT(x) 				std::cout << m_nom << " > " << x;
#else
#define DEBUG_ROBOT_PRINTLN(x)
#define DEBUG_ROBOT_PRINT(x)				
#endif

#define SIZE_BUFFER_RX	1000

#define NB_FLUSH_MIN	10
#define TEMP_MAX_FLUSH	1000

#define LOW 	0
#define HIGH 	1

//------------ CMD -------------------//
#define AUCUN							0
#define LIDAR_CMD_DEBUG               	1
#define LIDAR_CMD_START               	2
#define LIDAR_CMD_STOP                	3
#define LIDAR_CMD_GET_DISTANCE        	4
#define LIDAR_CMD_GET_ANGLE           	5
#define LIDAR_CMD_GET_STARTBIT        	6
#define LIDAR_CMD_GET_QUALITY         	7
#define LIDAR_CMD_SET_AUTOSTART       	8
#define LIDAR_CMD_RESET_AUTOSTART     	9
#define LIDAR_CMD_GET_DATA_AVAILABLE    10
#define LIDAR_CMD_GET_DETECTED_POINTS   11
#define LIDAR_CMD_GET_RAW_POINT         12
#define LIDAR_CMD_SET_SPEED             20
#define LIDAR_CMD_GET_SPEED             21
#define ACT_CMD_SERVO 					22
#define ACT_CMD_SET_MOT4QVit			23
#define ACT_CMD_SET_MOT 				25
#define ACT_CMD_CUR 					26
#define ACT_CMD_COLOR 					27
#define ACT_CMD_UART_SEND				28
#define ACT_CMD_RUPT					29
#define ACT_CMD_AX12					30
#define ACT_CMD_DIST					31
#define CMD_RST 						32
#define CMD_PING						33
#define HMI_CMD_POS						34
#define HMI_CMD_TXT						35
#define HMI_CMD_SCORE					36
#define HMI_RET_COTE					37
#define HMI_RET_OFF_PI					38
#define ACT_CMD_SEUIL_COLOR             39
#define ACT_CMD_RESET_CPT_COLOR         40
#define MSG_NON_PRIS_EN_CHARGE			41
#define LIDAR_RET_DEBUG_DEBUG       	42 
#define LIDAR_RET_DEBUG_START       	43 
#define LIDAR_RET_DEBUG_STOP        	44
#define ACT_CMD_SET_MOT4QPos			45
#define LIDAR_RET_DATA_AVAILABLE    	100
#define LIDAR_RET_DETECTED_POINTS   	101
#define LIDAR_RET_RAW_POINT         	102
#define LIDAR_RET_SPEED             	121

class Robot {
	public:
	    //functions
		Robot(std::string nom, SPI *pSpi, uint8_t id);
		virtual ~Robot();
		void stopThreadDetection();
		bool isContinueThread();
		void reset();
		void sendSPI(uint8_t *buf, uint8_t bufSize);
		void sendReceiveSPI(uint8_t data);
		void flush(uint16_t nb);
		void GetPing();
		bool Ping();
		void checkMessages();
		virtual void DecodMsg(uint8_t buf[]);
		bool isConnected();

		int16_t x = 1500,y = 1000, t = 45;
		uint8_t bufferRx[SIZE_BUFFER_RX];
		uint8_t iRxIn = 0;
		uint8_t iRxOut = 0;
		uint8_t nbMsgReceived = 0;
		bool receivingMsg = false;
		uint8_t currentMsgSize = 0;
		uint8_t nbBytesReceived = 0;
		uint32_t nbBytesReceivedTotal = 0;
    protected:
		uint8_t m_id;	//id of this SPI slave
		bool m_continueThread;
		bool m_fillBuffer = false;
		SPI *m_pSpi;
		std::string m_nom;
		pthread_t m_thread;
		std::mutex m_mutex;
		bool m_ping = false;
		bool m_connected = false;
};

#endif // ROBOT_H
