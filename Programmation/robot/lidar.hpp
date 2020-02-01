#ifndef LIDAR_H
#define LIDAR_H

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <queue>
#include <unistd.h>
#include <iostream>
#include <pthread.h>

#include "SPI.hpp"
#include "web.hpp"
#include "robot.hpp"

#define CODE_VAR_DISTANCE   1
#define CODE_VAR_ANGLE      2

void* thread_Lidar(void *threadid);

class Lidar : public Robot {
	public:
		Lidar(std::string nom, SPI *pSpi, uint8_t id, Web *pWeb);
		~Lidar();
		void checkMessages();
		bool startThreadDetection();
		
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
	protected:
		Web *m_pWeb;	//pointer to SPI instance
		std::queue<pointFloat2d> m_qDetectedPoints;
};

#endif // LIDAR_H