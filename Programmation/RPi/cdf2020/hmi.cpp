#include "hmi.hpp"

HMI::HMI(SPI *pSpi,uint8_t id){
    m_pSpi = pSpi;
	m_id = id;

	flush(255);
	//ajouter ping();
}
HMI::~HMI(){

}

void HMI::checkMessages(){
	while(nbMsgReceived > 0){
		nbMsgReceived--;
		uint8_t msgSize = bufferRx[iRxOut];
		iRxOut++;
		for(int i=0;i<msgSize+5;i++) {
			std::cout << (int)(bufferRx[i]) << std::endl;
		}


		if(iRxOut == SIZE_BUFFER_RX){
			iRxOut = 0;
		}
		msgSize++; //suppr
		uint8_t buf[msgSize];
		buf[0] = msgSize;
		for(int i = 1; i < msgSize; i++){
			buf[i] = bufferRx[iRxOut];
			iRxOut++;
			if(iRxOut == SIZE_BUFFER_RX){
				iRxOut = 0;
			}
		}
		uint8_t checksum = 0;
		for(int i = 0; i < msgSize-1; i++){
			checksum += buf[i];
		}
		if(checksum != buf[msgSize-1]){
			std::cout << "Hmi > CHECKSUM ERROR ! (msgSize = " << (int)msgSize << " & iRxOut = " << (int)iRxOut << ")" << " & CS = " << (int)buf[msgSize-1] << std::endl;
		}
		else{	//Checksum ok
			switch(buf[1]){	//type of msg
				case HMI_RET_COTE:
					m_hmi_cote = buf[2];
					if(m_hmi_cote == 1) {
						std::cout << "cote droit" << std::endl;
					}
					if(m_hmi_cote == 2) {
						std::cout << "cote gauche" << std::endl;
					}
					break;
				case HMI_RET_OFF_PI:
					if(buf[2]) {
						m_stopPrgm = true;
						m_stopMain = true;
					}
					break;
				default:
					std::cout << "erreur message non pris en charge" << std::endl;
					break;
			}
		}
	}
}

void HMI::sendSPI(uint8_t *buf, uint8_t bufSize){	//add size & checksum
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

void HMI::sendReceiveSPI(uint8_t data){	//send & handle response
	//std::cout << "sent : " << (int)data << std::endl;		//for debug
	uint8_t buffer[1];
	buffer[0] = data;
	wiringPiSPIDataRW(SPI_CHANNEL, buffer, 1);

	//std::cout << "sent : " << (int)data << " / " << (int)buffer[0] << std::endl;		//for debug
	if(receivingMsg){
		bufferRx[iRxIn] = buffer[0];
		iRxIn++;
		if(iRxIn == SIZE_BUFFER_RX){
			iRxIn = 0;
		}
		nbBytesReceived++;
		nbBytesReceivedTotal++;
	}
	else{
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

/*int HMI::Send(int in){
	unsigned char buffer[1];
	buffer[0] = (unsigned char)in;
	wiringPiSPIDataRW(SPI_CHANNEL, buffer, 1);
	//std::cout << "entree = " << in << "   /   " << " reponse = " << (int)buffer[0] << std::endl;
	//delay(SPI_DELAY_MS);
	delayMicroseconds(SPI_DELAY_US);
	return (int)buffer[0];
	
}

void HMI::SetPrgm(int in, std::string txt) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	unsigned int i;
	int check = 0;
	char txt_char[txt.size()+1];
	strcpy(txt_char, txt.c_str());
	if(txt.size() < 16) {
		Send(2+txt.size());
		check = 2+txt.size();
		Send(in);
		check += in;
		check %= 256;
		for(i=0;i<txt.size();i++) {
			Send(txt_char[i]);
			check += txt_char[i];
			check %= 256;
		}
		Send(check);
	} else {
		std::cout << "chaine trop longue max 15" << std::endl;
	}
	m_pSpi->unlock();

}*/

void HMI::SetPos(int in, int pos) {
	uint8_t buffer[3];
	if(in < 0 || in > 2) {
		std::cout << "erreur in = " << in << std::endl;
	} else {
		buffer[0] = HMI_CMD_POS+in;
		buffer[1] = (uint8_t)(pos/256);
		buffer[2] = pos%256;
		sendSPI(buffer,3);
	}
}

void HMI::EraseScreen(int in_era) {
	uint8_t buffer[197];
	int i_era;
	buffer[0] = HMI_CMD_TXT+in_era;
	for(i_era=0;i_era<196;i_era++) {
		buffer[i_era] = '\0';
	}
	sendSPI(buffer,197);
}

void HMI::flush(uint16_t nbBytes){
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

void HMI::SetPosXY(int pos_x, int pos_y) {
	uint8_t buffer[3];
	buffer[0] = HMI_CMD_POS;
	buffer[1] = (uint8_t)(pos_x/256);
	buffer[2] = pos_x%256;
	sendSPI(buffer,3);
	buffer[0] = HMI_CMD_POS+1;
	buffer[1] = (uint8_t)(pos_y/256);
	buffer[2] = pos_y%256;
	sendSPI(buffer,3);
}

void HMI::SetTxt(int in, std::string txt) {
	EraseScreen(in);
	delay(EraseTime);
	uint8_t buffer[txt.size()+2];
	char txt_char[txt.size()+1];
	int i;
	if(txt.size() < 197) {
		strcpy(txt_char, txt.c_str());
		buffer[0] = HMI_CMD_TXT+in;
		for(i=0;i<txt.size()+1;i++) {
			buffer[i+1] = txt_char[i];
		}
		sendSPI(buffer,txt.size()+2);
	} else {
		std::cout << "erreur chaine trop longue max 196" << std::endl;
	}
}

void HMI::SetTxtFull(std::string txt_in) {
	if(txt_in.size() < 197) {
		SetTxt(1, txt_in.substr(0,txt_in.size()));
		EraseScreen(2);
		EraseScreen(3);
		EraseScreen(4);
	} else if(txt_in.size() < 392) {
		SetTxt(1, txt_in.substr(0,196));
		SetTxt(2, txt_in.substr(196,(txt_in.size()-196)));
		EraseScreen(3);
		EraseScreen(4);
	} else if(txt_in.size() < 588) {
		SetTxt(1, txt_in.substr(0,196));
		SetTxt(2, txt_in.substr(196,196));
		SetTxt(3, txt_in.substr(392,txt_in.size()-392));
		EraseScreen(4);
	} else if(txt_in.size() < 784) {
		SetTxt(1, txt_in.substr(0,196));
		SetTxt(2, txt_in.substr(196,196));
		SetTxt(2, txt_in.substr(392,196));
		SetTxt(3, txt_in.substr(588,txt_in.size()-588));
	} else {
		std::cout << "erreur chaine trop longue";
	}
}

void HMI::setScore(int score) {
	uint8_t buffer[3];
	buffer[0] = HMI_CMD_SCORE;
	buffer[1] = score %256;
	buffer[2] = (uint8_t)(score /256);
	sendSPI(buffer,3);
}

//gestion du thread

bool HMI::startThreadDetection(){
	m_mutex.lock();
	m_continueThread = true;
	m_mutex.unlock();
	int rc = pthread_create(&m_thread, NULL, thread_HMI, this);
	if (rc) {
		std::cout << "Error:unable to create thread," << rc << std::endl;
		return false;
    }
	return true;
}

void HMI::stopThreadDetection(){
	m_mutex.lock();
	m_continueThread = false;
	m_mutex.unlock();
}

void* thread_HMI(void *threadid){
	HMI *hmi = (HMI*)threadid;

	while(hmi->isContinueThread()){
		//hmi->sendGetDetectedPoints();
		hmi->flush(5);
		hmi->checkMessages();
		delay(1);
		
		//delay(10);
	}
	
	pthread_exit(NULL);
}

bool HMI::isContinueThread(){
	m_mutex.lock();
	bool b = m_continueThread;
	m_mutex.unlock();
	return b;
}

void HMI::resetPic(void){
	uint8_t buffer[1];
	buffer[0] = HMI_CMD_RST_UC;
	sendSPI(buffer,1);
}

bool HMI::isStopPrgm(void) {
	m_mutex.lock();
	bool b = m_stopPrgm;
	m_mutex.unlock();
	return b;
}

bool HMI::isStopMain(void) {
	m_mutex.lock();
	bool b = m_stopMain;
	m_mutex.unlock();
	return b;
}

uint8_t HMI::CoteChoisi(void) {
	m_mutex.lock();
	uint8_t b = m_hmi_cote;
	m_mutex.unlock();
	return b;
}