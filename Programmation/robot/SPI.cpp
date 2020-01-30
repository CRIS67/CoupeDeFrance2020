#include "SPI.hpp"

SPI::SPI(int channel, int speed){
	//wiringPiSetup();
	m_channel = channel;
    m_fd = wiringPiSPISetup(channel, speed);
	pinMode(PIN_MUX_A, OUTPUT);
	pinMode(PIN_MUX_B, OUTPUT);
	pinMode(PIN_MUX_C, OUTPUT);
	
	for(int i=0;i<NB_CARTE;i++) {
		PinRst
		pinMode(PinRst[i], OUTPUT);
		digitalWrite(PinRst[i],HIGH);
	}

	m_slaveId = -1;
}
SPI::~SPI(){

}
/*Change slave select line*/
void SPI::setSlave(uint8_t id){
	if(id < NB_CARTE){
		m_slaveId = id;
		digitalWrite(PIN_MUX_A,(id & 0x1));			//bit0 of id -> MUX_A
		digitalWrite(PIN_MUX_B,((id >> 1) & 0x1));	//bit1 of id -> MUX_B
		digitalWrite(PIN_MUX_C,((id >> 2) & 0x1));	//bit2 of id -> MUX_C
		//std::cout << "slave set to = " << (int)m_slaveId << std::endl;
	}
}
int SPI::getSlaveId(){
	return m_slaveId;
}
/*Lock the SPI bus from preventing multiple acces from other threads*/
void SPI::lock(){
	m_mutex.lock();
}
/*Unlock the SPI bus*/
void SPI::unlock(){
	m_mutex.unlock();
}
int SPI::getFd(){
	return m_fd;
}
int SPI::getChannel(){
	return m_channel;
}