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

#include "SPI.hpp"
#include "web.hpp"

#define DEBUG_ROBOT_ENABLE_PRINT		1

#if(DEBUG_ROBOT_ENABLE_PRINT)
#define DEBUG_ROBOT_ENABLE_PRINT(x) 				std::cout << m_nom << " Debug> " << x << std::endl;
#else
#define DEBUG_ROBOT_ENABLE_PRINT(x)				
#endif

#define SIZE_BUFFER_RX	1000

#define NB_FLUSH_MIN	4
#define TEMP_MAX_FLUSH	1000

#define ERASE_TIME 100
#define EraseTime 100

#define LOW 	0
#define HIGH 	1

#define CODE_VAR_DISTANCE   1
#define CODE_VAR_ANGLE      2

//------------ CMD -------------------//
#define AUCUNE							0
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
#define ACT_CMD_SET_MOT4Q 				23
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
#define LIDAR_RET_DEBUG_DEBUG       	42 
#define LIDAR_RET_DEBUG_START       	43 
#define LIDAR_RET_DEBUG_STOP        	44
#define LIDAR_RET_DATA_AVAILABLE    	100
#define LIDAR_RET_DETECTED_POINTS   	101
#define LIDAR_RET_RAW_POINT         	102
#define LIDAR_RET_SPEED             	121

#define SERVO_VALUE_HIGH	700
#define SERVO_VALUE_MIDDLE	1000
#define SERVO_VALUE_DROP	1500
#define SERVO_VALUE_LOW		1600

void* thread(void *threadid); //à voir

class Robot {
	public:
	    //functions
		Robot(std::string nom, SPI *pSpi, uint8_t id, int nb_servo, int nb_moteur4Q, int nb_moteur, int nb_capt_cur, int nb_capt_couleur,
              int nb_uart, int nb_rupteur, int nb_ax12, int nb_lidar, int nb_screen, int nb_capt_dist);
		virtual ~Robot();
		void checkMessages();
		bool startThreadDetection();
		void stopThreadDetection();
		bool isContinueThread();
		void reset();
		void sendSPI(uint8_t *buf, uint8_t bufSize);
		void sendReceiveSPI(uint8_t data);
		void flush(uint16_t nb);
		bool flushDyn();
		void GetPing();
		bool GetPing();

		//actuator
		void MoveServo(int nb_bras, int pos);
		void SetMot(int nb_bras, int state);
		void GetColor(int nb_bras);
		void GetCurrent(int nb_bras);
		void GetColor(int nb_bras);
		void GetRupt(int nb_bras);
		void GetDist(int nb_bras);
		bool flushDyn(void);
		int Cur(int nb_bras);
		int Color(int nb_bras);
		int Dist(int nb_bras);
		int Rupt(int nb_bras);
		void SetMot4Q(int nb_bras, int vit, int sens);

		//hmi
		uint8_t CoteChoisi();
		bool isStopPrgm();
		bool isStopMain();
		void SetPos(int in, int pos);
		void EraseScreen(int in_era);
		void SetTxt(int in, std::string txt);
		void SetTxtFull(std::string txt_in);

		//lidar
		void start();
		void stop();
		void getAvailableData();
		void getRawPoint();
		void sendGetDetectedPoints();
		void setSpeed(uint8_t speed);
		void getSpeed();
		void addDetectedPoint(pointFloat2d p);
		std::queue<pointFloat2d> getDetectedPoints();
		std::queue<pointFloat2d> getAndClearDetectedPoints();
		void setFillBuffer(bool b);
		bool getFillBuffer();

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
        int m_nb_servo;
        int m_nb_moteur4Q;
        int m_nb_moteurs;
        int m_nb_capt_cur;
        int m_nb_capt_couleur;
        int m_nb_uart;
        int m_nb_rupteur;
        int m_nb_ax12;
        int m_nb_lidar;
        int m_nb_screen;
        int m_nb_capt_dist;
		uint8_t m_id;	//id of this SPI slave
		bool m_continueThread;
		bool m_fillBuffer = false;
		bool m_stopPrgm = false;
		bool m_stopMain = false;
		SPI *m_pSpi;
		std::string m_nom;
		Web *m_pWeb;
        int m_cur[10];
        int m_color[10];
        int m_dist[10];
        int m_RX[10];
        int m_rupt[10];
		bool m_stopPrgm = false;
		bool m_stopMain = false;
		uint8_t m_hmi_cote = 0;
		pthread_t m_thread;
		std::mutex m_mutex;
		std::queue<pointFloat2d> m_qDetectedPoints;
		bool m_ping = false;
};

#endif // ROBOT_H
