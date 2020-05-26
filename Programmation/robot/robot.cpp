#include "robot.hpp"

Robot::Robot() {}

Robot::Robot(std::string nom, SPI *pSpi, uint8_t id) {
	m_nom = nom;
	m_pSpi = pSpi;
	m_id = id;
	int cpt = 0;
	GetPing();
	while(cpt < 2000 && !m_ping) {
		delay(1);
		cpt++;
		checkMessages();
	}
	if(m_ping) {
		DEBUG_ROBOT_PRINTLN(" connecté")
		m_connected = true;
	} else {
		reset();
		delay(100);
		cpt = 0;
		GetPing();
		while(cpt < 2000 && !m_ping) {
			delay(1);
			cpt++;
			checkMessages();
		}
		if(m_ping) {
			DEBUG_ROBOT_PRINTLN(" connecté (2)")
			m_connected = true;
		} else {
			DEBUG_ROBOT_PRINTLN(" non connecté ERROR")
			m_connected = false;
		}
	}
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

	//std::cout << "sent : " << (int)data << " / " << (int)buffer[0] << std::endl;		//for debug
	delay(2);
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

void Robot::checkMessages() {
	while(nbMsgReceived > 0) {
		nbMsgReceived--;
		uint8_t msgSize = bufferRx[iRxOut];
		int i;
		iRxOut++;
		if(iRxOut == SIZE_BUFFER_RX){
			iRxOut = 0;
		}
		uint8_t buf[msgSize];
		buf[0] = msgSize;
		for(i = 1; i < msgSize; i++){
			buf[i] = bufferRx[iRxOut];
			iRxOut++;
			if(iRxOut == SIZE_BUFFER_RX){
				iRxOut = 0;
			}
		}
		uint8_t checksum = 0;
		for(i = 0; i < msgSize-1; i++){
			checksum += buf[i];
		}

		//for(int i = 0; i < msgSize-1; i++){std::cout << "MSG" << (int)i << " " << (int)buf[i] << std::endl;}std::cout << "MSG" << (int)msgSize << " " << (int)checksum << std::endl;

		if(checksum != buf[msgSize-1]){
			DEBUG_ROBOT_PRINTLN("CHECKSUM ERROR ! (msgSize = " << (int)msgSize << " & iRxOut = " << (int)iRxOut << ")" << " & CS = " << (int)buf[msgSize-1] << " CS th = " << (int)checksum)
			m_pSpi->lock();
			if(m_pSpi->getSlaveId() != m_id){	
				m_pSpi->setSlave(m_id);		//change Chip select
			}
			uint8_t b[1];
			for(i = 0; i < 255; i++){
				b[0] = 0;
				wiringPiSPIDataRW(SPI_CHANNEL, b, 1);
				delayMicroseconds(SPI_DELAY_US);
			}
			m_pSpi->unlock();
		} else {	//Checksum ok
			if(buf[1] == CMD_PING) {
				if(buf[3] == 0x01) {
					m_ping = true;
				} else {
					m_ping = false;
				}
			} else if(buf[1] == MSG_NON_PRIS_EN_CHARGE){
				DEBUG_ROBOT_PRINTLN("Salve receive msg with not concern it !!! ERROR" << buf[1])
			} else {
				DecodMsg(buf);
			}
		}
	}
}

void Robot::DecodMsg(uint8_t buf[]) {}

bool Robot::isConnected(void) {
	m_mutex.lock();
	bool b = m_connected;
	m_mutex.unlock();
	return b;
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
	delay(2000);
}

void Robot::GetPing() {
	uint8_t buffer[1];
	buffer[0] = CMD_PING;
	sendSPI(buffer,1);
	flush(NB_FLUSH_MIN);
}

bool Robot::Ping(void) {
	m_mutex.lock();
	bool b = m_ping;
	m_ping = false;
	m_mutex.unlock();
	return b;
}

std::string Robot::GetName(void) {
	return m_nom;
}