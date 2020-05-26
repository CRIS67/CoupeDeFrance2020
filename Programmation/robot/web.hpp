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
#include <fstream>
#include <ctime>
#include "dspic.hpp"
#include "actuator.hpp"


#define DEBUG_PID	0

const int Motor4qVitUp[][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {200, 150, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
const int Motor4qVitDown[][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {115, 150, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

void* thread_HandleConnnection(void *threadid);
std::string simulateResponse(double i);

struct pointFloat2d{
  float x;
  float y;
};

class Web
{
    public:
        Web(DsPIC *ds, Actuator a[], int leng);
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
		Actuator *act[10];
		bool waitingResponsePID = false;
		std::ofstream ElementFile, ActionFile;
		std::ifstream ElementFileR, ActionFileR;
		float ActTime = 0;
		
		void addLidarPoints(float x, float y);
		void addLidarPoints(pointFloat2d fp);
		void addLidarPoints(std::vector<pointFloat2d> vect_fp);
		void addElementPoints(float x, float y);
		void addElementPoints(pointFloat2d fp);
		void addElementAttribut(int val);
		void clearLidarPoints();
		bool m_clearLidarPoints = false;
		bool m_clearElementsPoints = false;
		bool m_radarScan = true;
		bool m_ElementCreateEnd = false;
		bool m_rec = false;
		std::queue<pointFloat2d> lidarPoints, ElementPoints;
		std::queue<int> ElementAttributs;
		
    int m_mot4q_pos[3][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    protected:
        int socket_listen;
        int socket_client;
        pthread_t threads;
		bool m_continueThread;
		std::mutex m_mutex;
		bool m_record = false;
    private:
};

std::string realResponse(Web *w);

#endif // WEB_H
