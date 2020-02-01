#include "lidar.hpp"

Lidar::Lidar(std::string nom, SPI *pSpi, uint8_t id, Web *pWeb) : Robot(nom, pSpi, id) {
	m_pWeb = pWeb;
	DEBUG_ROBOT_PRINT("Lidar");
}

Lidar::~Lidar() {}

bool Lidar::startThreadDetection(){
	m_mutex.lock();
	m_continueThread = true;
	m_mutex.unlock();
	int rc = pthread_create(&m_thread, NULL, thread_Lidar, this);
	if (rc) {
		std::cout << "Error:unable to create thread," << rc << std::endl;
		return false;
    }
	return true;
}

void* thread_Lidar(void *threadid){
	Lidar *lidar = (Lidar*)threadid;
	while(lidar->isContinueThread()){
		lidar->sendGetDetectedPoints();
		lidar->flush(255);
		lidar->checkMessages();
		delay(1);
	}
	pthread_exit(NULL);
}

void Lidar::checkMessages() {
	while(nbMsgReceived > 0) {
		nbMsgReceived--;
		uint8_t msgSize = bufferRx[iRxOut];
		iRxOut++;
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
			std::cout << m_nom << " > CHECKSUM ERROR ! (msgSize = " << (int)msgSize << " & iRxOut = " << (int)iRxOut << ")" << " & CS = " << (int)buf[msgSize-1] << std::endl;
		} else {	//Checksum ok
			switch(buf[1]){	//type of msg
				case LIDAR_RET_DEBUG_DEBUG:
					//std::cout << "Lidar> Debug : debug received" << std::endl;
					DEBUG_ROBOT_PRINT(m_nom << "Debug received");
					break;
				case LIDAR_RET_DEBUG_START:
					//std::cout << "Lidar> Debug : Start received" << std::endl;
					DEBUG_ROBOT_PRINT(m_nom << "Start received");
					break;
				case LIDAR_RET_DEBUG_STOP:
					//std::cout << "Lidar> Debug : Stop received" << std::endl;
					DEBUG_ROBOT_PRINT(m_nom << "Stop received");
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
					std::cout << "Lidar> Distance : " << distance << " & angle : " << angle << "& quality : " << (int)quality << std::endl;
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
						ptr[3] = buf[i*8+9];
						/*for(int i = 2; i < 10; i++){
							std::cout << "buf["<<i<<"] : " << (int)buf[i] << " / ";
						}*/
						//std::cout << "x : " << x << " & y : " << y << std::endl;
						//std::cout << x << "," << y << std::endl;
						//std::cout << "Lidar> " << p.x << "," << p.y << std::endl;
						//addDetectedPoint(p);
						double angle = atan2(p.y,p.x);
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
					break;}
				case CMD_PING:
					if(buf[3] == 0x01) {
						m_ping = true;
					} else {
						m_ping = false;
					}
					break;
				default:
					std::cout << "message non pris en charge" << std::endl;
					break;
			}
		}
	}
}

void Lidar::start() {
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_START;
	sendSPI(buffer,1);
}

void Lidar::stop() {
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_STOP;
	sendSPI(buffer,1);
}

void Lidar::getAvailableData() {
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_DATA_AVAILABLE;
	sendSPI(buffer,1);
}

void Lidar::getRawPoint() {
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_RAW_POINT;
	sendSPI(buffer,1);
}

void Lidar::sendGetDetectedPoints() {
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_DETECTED_POINTS;
	sendSPI(buffer,1);
}

void Lidar::setSpeed(uint8_t speed) {	//change rotation speed
	uint8_t buffer[2];
	buffer[0] = LIDAR_CMD_SET_SPEED;
	buffer[1] = speed;
	sendSPI(buffer,2);
}

void Lidar::getSpeed() {	//change rotation speed
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_SPEED;
	sendSPI(buffer,1);
}

void Lidar::addDetectedPoint(pointFloat2d p) {
	m_mutex.lock();
	m_qDetectedPoints.push(p);
	m_mutex.unlock();
}

std::queue<pointFloat2d> Lidar::getDetectedPoints() {
	m_mutex.lock();
	std::queue<pointFloat2d> q = m_qDetectedPoints;
	m_mutex.unlock();
	return q;
}

std::queue<pointFloat2d> Lidar::getAndClearDetectedPoints() {
	m_mutex.lock();
	std::queue<pointFloat2d> q;
	std::swap(m_qDetectedPoints,q);
	m_mutex.unlock();
	return q;
}
	
void Lidar::setFillBuffer(bool b) {
	m_mutex.lock();
	m_fillBuffer = b;
	m_mutex.unlock();
}

bool Lidar::getFillBuffer() {
	m_mutex.lock();
	bool b = m_fillBuffer;
	m_mutex.unlock();
	return b;
}