#include "robot.hpp"

Robot::Robot(std::string nom, SPI *pSpi, uint8_t id) {
	m_nom = nom;
	m_pSpi = pSpi;
	m_id = id;
}

Robot::~Robot() {}

void Robot::sendSPI(uint8_t *buf, uint8_t bufSize){	//add size & checksum
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	uint8_t b[1];
	b[0] = bufSize + 1;
	uint8_t checksum = b[0];
	sendReceiveSPI(b[0]);
	//delay(SPI_DELAY_MS);
	delayMicroseconds(SPI_DELAY_US);
	for(int i = 0; i < bufSize; i++){
		b[0] = buf[i];
		checksum += buf[i];
		sendReceiveSPI(b[0]);
		//delay(SPI_DELAY_MS);
		delayMicroseconds(SPI_DELAY_US);
	}
	b[0] = checksum;
	sendReceiveSPI(b[0]);
	//delay(SPI_DELAY_MS);
	delayMicroseconds(SPI_DELAY_US);
	m_pSpi->unlock();
}

void Robot::sendReceiveSPI(uint8_t data){	//send & handle response
	//std::cout << "sent : " << (int)data << std::endl;		//for debug
	uint8_t buffer[1];
	buffer[0] = data;
	wiringPiSPIDataRW(SPI_CHANNEL, buffer, 1);

	std::cout << "sent : " << (int)data << " / " << (int)buffer[0] << std::endl;		//for debug
	if(receivingMsg){
		bufferRx[iRxIn] = buffer[0];
		iRxIn++;
		if(iRxIn == SIZE_BUFFER_RX){
			iRxIn = 0;
		}
		nbBytesReceived++;
		nbBytesReceivedTotal++;
	}else{
		if(buffer[0] != 0){
			currentMsgSize = buffer[0];
			bufferRx[iRxIn] = currentMsgSize;
			iRxIn++;
			if(iRxIn == SIZE_BUFFER_RX){
				iRxIn = 0;
			}
			receivingMsg = true;
			nbBytesReceived++;
			nbBytesReceivedTotal++;
		}
	}
	if(nbBytesReceived == currentMsgSize && receivingMsg){	//message received
		receivingMsg = false;
		nbMsgReceived++;
		nbBytesReceived = 0;
	}
}

void Robot::flush(uint16_t nbBytes){
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	uint8_t buffer[1];
	for(uint16_t i = 0; i < nbBytes; i++){
		buffer[0] = 0;
		sendReceiveSPI(buffer[0]);
		//delay(SPI_DELAY_MS);
		delayMicroseconds(SPI_DELAY_US);
		delay(10);
	}
	m_pSpi->unlock();
}

bool Robot::isContinueThread(){
	m_mutex.lock();
	bool b = m_continueThread;
	m_mutex.unlock();
	return b;
}

void Robot::stopThreadDetection(){
	m_mutex.lock();
	m_continueThread = false;
	m_mutex.unlock();
}

void Robot::reset(void) {
	//soft
	uint8_t buffer[1];
	buffer[0] = CMD_RST;
	sendSPI(buffer,1);
	//hard
	digitalWrite(PinRst[m_id],LOW);
	delay(200);
	digitalWrite(PinRst[m_id],HIGH);
	delay(500);
}

void Robot::GetPing() {
	uint8_t buffer[1];
	buffer[0] = CMD_PING;
	sendSPI(buffer,1);
	flush(5);
}

bool Robot::Ping(void) {
	m_mutex.lock();
	bool b = m_ping;
	m_ping = false;
	m_mutex.unlock();
	return b;
}