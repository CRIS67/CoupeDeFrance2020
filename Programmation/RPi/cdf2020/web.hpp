#ifndef WEB_H
#define WEB_H

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <string.h>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include "dspic.hpp"
#include "actuators.hpp"

#define DEBUG_PID	0

struct pointFloat2d{
  float x;
  float y;
};

void* thread_HandleConnnection(void *threadid);
std::string simulateResponse(double i);
class Web
{
    public:
        Web(DsPIC *ds, Actuators *a1, Actuators *a2);
        virtual ~Web();
        bool acceptClient();
		void closeClient();
        bool sendMsg(std::string message);
		std::string receiveMsg();
        bool startThread();
        void stopThread();
        bool isContinueThread();
		
        std::string s;
		DsPIC *dspic;
		Actuators *actFront;
		Actuators *actBack;
		bool waitingResponsePID = false;
		
		void addLidarPoints(float x, float y);
		void addLidarPoints(pointFloat2d fp);
		void addLidarPoints(std::vector<pointFloat2d> vect_fp);
		void clearLidarPoints();
		bool m_clearLidarPoints = false;
		bool m_radarScan = true;
		std::queue<pointFloat2d> lidarPoints;
		
    protected:
        int socket_listen;
        int socket_client;
        pthread_t threads;
		bool m_continueThread;
		std::mutex m_mutex;
    private:
};

std::string realResponse(Web *w);

#endif // WEB_H
