#include "lidar.hpp"

Lidar::Lidar(std::string nom, SPI *pSpi, uint8_t id, Web *pWeb) : Robot(nom, pSpi, id) {
	m_pWeb = pWeb;
	std::cout << std::endl;
}

Lidar::~Lidar() {}

bool Lidar::startThreadDetection(){
	m_mutex.lock();
	m_continueThread = true;
	m_mutex.unlock();
	int rc = pthread_create(&m_thread, NULL, thread_Lidar, this);
	if (rc) {
    	DEBUG_ROBOT_PRINTLN("Error:unable to create thread," << rc)
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

void Lidar::DecodMsg(uint8_t buf[]) {
	switch(buf[1]){	//type of msg
		case LIDAR_RET_DEBUG_DEBUG:
			//std::cout << "Lidar> Debug : debug received" << std::endl;
			DEBUG_ROBOT_PRINTLN("Debug received");
			break;
		case LIDAR_RET_DEBUG_START:
			//std::cout << "Lidar> Debug : Start received" << std::endl;
			DEBUG_ROBOT_PRINTLN("Start received");
			break;
		case LIDAR_RET_DEBUG_STOP:
			//std::cout << "Lidar> Debug : Stop received" << std::endl;
			DEBUG_ROBOT_PRINTLN("Stop received");
			break;
		case LIDAR_RET_DATA_AVAILABLE:
    		DEBUG_ROBOT_PRINTLN("Data available = " << (int)buf[2])
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
    		DEBUG_ROBOT_PRINTLN("Distance : " << distance << " & angle : " << angle << "& quality : " << (int)quality)
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
    			DEBUG_ROBOT_PRINT("buf["<<i<<"] : " << (int)buf[i] << " / ")
			}
    		DEBUG_ROBOT_PRINTLN("speed : " << speed)
			break;}
		default:
    		DEBUG_ROBOT_PRINTLN("message non pris en charge")
			break;
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