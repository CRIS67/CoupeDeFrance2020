#include "hmi.hpp"

bool stopMain = false;
bool turnOffPi = false;

HMI::HMI(SPI *pSpi,uint8_t id){
    m_pSpi = pSpi;
	m_id = id;

	flush(255);
	//ajouter ping();
}
HMI::~HMI(){

}

void HMI::SendManual(void) {
	unsigned char buffer[100];
	int in = 0;
	while(in != -1) {
		std::cout << ">";
		std::cin >> in;
		if(in > -1 && in < 256) {
			buffer[0] = (unsigned char)in;
			wiringPiSPIDataRW(SPI_CHANNEL, buffer, 1);
			std::cout << "reponse = " << (int)buffer[0] << std::endl;
			delay(10);
		} else {
			in = -1;
		}
	}
}
int HMI::Send(int in){
	unsigned char buffer[1];
	buffer[0] = (unsigned char)in;
	wiringPiSPIDataRW(SPI_CHANNEL, buffer, 1);
	//std::cout << "entree = " << in << "   /   " << " reponse = " << (int)buffer[0] << std::endl;
	//delay(SPI_DELAY_MS);
	delayMicroseconds(SPI_DELAY_US);
	return (int)buffer[0];

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
				case HMI_SHUTDOWN_PI:
					//std::cout << "Lidar> Debug : debug received" << std::endl;
					if(buf[2]) {
						//setTurnOffPi();
						//setStopMain();
					}
					break;
				/*case LIDAR_RET_DEBUG_START:
					//std::cout << "Lidar> Debug : Start received" << std::endl;
					DEBUG_LIDAR_PRINT("Start received");
					break;
				case LIDAR_RET_DEBUG_STOP:
					//std::cout << "Lidar> Debug : Stop received" << std::endl;
					DEBUG_LIDAR_PRINT("Stop received");
					break;
				case LIDAR_RET_DATA_AVAILABLE:
					std::cout << "Lidar> Data available = " << (int)buf[2]  << std::endl;
					break;
				case LIDAR_RET_RAW_POINT:{
					float distance;
					float *dPtr = &distance;
					uint8_t *ptr = (uint8_t*)dPtr;
					ptr[0] = buf[2];
					ptr[1] = buf[3];
					ptr[2] = buf[4];
					ptr[3] = buf[5];
					float angle;
					dPtr = &angle;
					ptr = (uint8_t*)dPtr;
					ptr[0] = buf[6];
					ptr[1] = buf[7];
					ptr[2] = buf[8];
					ptr[3] = buf[9];
					
					uint8_t quality = buf[10];
					/*for(int i = 2; i < 10; i++){
						std::cout << "buf["<<i<<"] : " << (int)buf[i] << " / ";
					}*/
					/*std::cout << "Lidar> Distance : " << distance << " & angle : " << angle << "& quality : " << (int)quality << std::endl;
					break;}
				case LIDAR_RET_DETECTED_POINTS:{
					uint8_t s = buf[0];
					uint8_t nbPoints = s/8;
					//std::cout << "s : " << (int)s << " & nbPoints : " << (int)nbPoints << std::endl;
					for(int i =0; i < nbPoints; i++){
						pointFloat2d p;
						//float x,y;
						float *dPtr = &p.x;
						uint8_t *ptr = (uint8_t*)dPtr;
						ptr[0] = buf[i*8+2];
						ptr[1] = buf[i*8+3];
						ptr[2] = buf[i*8+4];
						ptr[3] = buf[i*8+5];
						dPtr = &p.y;
						ptr = (uint8_t*)dPtr;
						ptr[0] = buf[i*8+6];
						ptr[1] = buf[i*8+7];
						ptr[2] = buf[i*8+8];
						ptr[3] = buf[i*8+9];*/
						/*for(int i = 2; i < 10; i++){
							std::cout << "buf["<<i<<"] : " << (int)buf[i] << " / ";
						}*/
						//std::cout << "x : " << x << " & y : " << y << std::endl;
						//std::cout << x << "," << y << std::endl;
						//std::cout << "Lidar> " << p.x << "," << p.y << std::endl;
						//addDetectedPoint(p);
						/*double angle = atan2(p.y,p.x);
						double distance = sqrt(p.x*p.x + p.y*p.y);
						//angle += m_pWeb->dspic->getT() + 3.14159/4 + 3.14159;
						angle += m_pWeb->dspic->getT() + 3.14159/4;
						p.x = distance*cos(angle);
						p.y = distance*sin(angle);
						p.x += m_pWeb->dspic->getX();
						p.y += m_pWeb->dspic->getY();
						m_pWeb->addLidarPoints(p);
						if(getFillBuffer()){
							addDetectedPoint(p);
						}
					}
					
					break;}
				case LIDAR_RET_SPEED:{
					float speed;
					float *dPtr = &speed;
					uint8_t *ptr = (uint8_t*)dPtr;
					ptr[0] = buf[2];
					ptr[1] = buf[3];
					ptr[2] = buf[4];
					ptr[3] = buf[5];
					for(int i = 2; i < 6; i++){
						std::cout << "buf["<<i<<"] : " << (int)buf[i] << " / ";
					}
					std::cout << "speed : " << speed << std::endl;
					break;}*/
			}
		}
	}
}

/*void HMI::setStopMain(){
	m_mutex.lock();
	m_stopMain = true;
	m_mutex.unlock();
}

void HMI::setTurnOffPi(){
	m_mutex.lock();
	m_turnOffPi = true;
	m_mutex.unlock();
}*/

/*void HMI::sendSPI(uint8_t *buf, uint8_t bufSize){	//add size & checksum
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
}*/
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

}
void HMI::SetPos(int in, int pos) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	in += 6;
	int lsb = pos%256, msb = (int)(pos/256);
	Send(4);
	Send(in);
	Send(msb);
	Send(lsb);
	Send(4+in+msb+lsb);
	m_pSpi->unlock();
}
void HMI::EraseScreen(int in_era) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	int check_era = 198, i_era;
	Send(198);
	in_era += 7;
	Send(in_era);
	check_era += in_era;
	check_era %= 256;
	for(i_era=0;i_era<196;i_era++) {
		Send('\0');
	}
	Send(check_era);
	m_pSpi->unlock();
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
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	int lsb = pos_x%256, msb = (int)(pos_x/256);
	Send(4);
	Send(6);
	Send(msb);
	Send(lsb);
	Send(10+lsb+msb);
	lsb = pos_y%256;
	msb = (int)(pos_y/256);
	Send(4);
	Send(7);
	Send(msb);
	Send(lsb);
	Send(11+lsb+msb);
	m_pSpi->unlock();
}

void HMI::SetTxt(int in, std::string txt) {
	EraseScreen(in);
	delay(EraseTime);
	int check = 0;
	in += 7;
	char txt_char[txt.size()+1];
	strcpy(txt_char, txt.c_str());
	if(txt.size() < 197) {
		Send(2+txt.size());
		check = 2+txt.size();
		Send(in);
		check += in;
		check %= 256;
		for(unsigned int i=0;i<txt.size();i++) {
			Send(txt_char[i]);
			check += txt_char[i];
			check %= 256;
		}
		Send(check);
	} else {
		std::cout << "erreur chaine trop longue max 196" << std::endl;
	}
}

void HMI::SetTxtFull(std::string txt_in) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	if(txt_in.size() < 197) {
		SetTxt(1, txt_in.substr(0,txt_in.size()));
		delay(EraseTime);
		EraseScreen(2);
		delay(EraseTime);
		EraseScreen(3);
		delay(EraseTime);
		EraseScreen(4);
	} else if(txt_in.size() < 392) {
		SetTxt(1, txt_in.substr(0,196));
		delay(EraseTime);
		SetTxt(2, txt_in.substr(196,(txt_in.size()-196)));
		delay(EraseTime);
		EraseScreen(3);
		delay(EraseTime);
		EraseScreen(4);
	} else if(txt_in.size() < 588) {
		SetTxt(1, txt_in.substr(0,196));
		delay(EraseTime);
		SetTxt(2, txt_in.substr(196,196));
		delay(EraseTime);
		SetTxt(3, txt_in.substr(392,txt_in.size()-392));
		delay(EraseTime);
		EraseScreen(4);
	} else if(txt_in.size() < 784) {
		SetTxt(1, txt_in.substr(0,196));
		delay(EraseTime);
		SetTxt(2, txt_in.substr(196,196));
		delay(EraseTime);
		SetTxt(2, txt_in.substr(392,196));
		delay(EraseTime);
		SetTxt(3, txt_in.substr(588,txt_in.size()-588));
	} else {
		std::cout << "erreur chaine trop longue";
	}
	m_pSpi->unlock();
}
void HMI::setScore(int score) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	uint8_t lsb = score %256;
	uint8_t msb = (uint8_t)(score /256);
	Send(4);
	Send(13);
	Send(msb);
	Send(lsb);
	Send((uint8_t)(17+msb+lsb));

	m_pSpi->unlock();
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
		hmi->flush(255);
		hmi->checkMessages();
		delay(10);
		
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
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	Send(2);
	Send(14);
	Send(16);
	m_pSpi->unlock();
}