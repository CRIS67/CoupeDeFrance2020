#ifndef SPI_H
#define SPI_H

#define SPI_CHANNEL 0		//Use Chip Select CE0
#define SPI_SPEED	250000	//0.5MHz
//#define SPI_DELAY_MS	10	//100ms
#define SPI_DELAY_US	0	//100us

#define PIN_MUX_A	28		//pin 38 / BCM 20 / WiringPi 28
#define PIN_MUX_B	29		//pin 40 / BCM 21 / WiringPi 29
#define PIN_MUX_C	26

#define HIGH 	1

#define SPI_ID_LIDAR		0
#define SPI_ID_ACT_BACK		1
#define SPI_ID_ACT_SCARA	2
#define SPI_ID_HMI			3
#define SPI_ID_XBEE			4

#define NB_CARTE			5

const int PinRst[] = {22, 24, 25, 23, 21}; //lidar, act0, act1, hmi, xbee

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <mutex>

class SPI
{
    public:
        SPI(int channel, int speed);
        virtual ~SPI();
		void setSlave(uint8_t id);
		void lock();
		void unlock();
		int getFd();
		int getChannel();
		int getSlaveId();
    protected:
    private:
		int m_fd;			//SPI file descriptor
		int m_channel;		//SPI channel
		uint8_t m_slaveId;	//ID of selected slave
		std::mutex m_mutex;	//SPI file descriptor
};

#endif // SPI_H
