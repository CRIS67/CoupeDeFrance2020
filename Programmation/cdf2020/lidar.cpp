#include "lidar.hpp"

Lidar::Lidar(SPI *pSpi,uint8_t id, Web *pWeb){
	//fd = wiringPiSPISetup(CHANNEL, SPI_FREQUENCY);
	m_pSpi = pSpi;
	m_pWeb = pWeb;
	m_id = id;

	/*Initialize*/
	flush(255);
	setSpeed(150);
	iRxIn = 0;
	iRxOut = 0;
	receivingMsg = false;
	currentMsgSize = 0;
	nbBytesReceived = 0;
	nbBytesReceivedTotal = 0;
	nbMsgReceived = 0;
	//rajouter ping()
	//start(); 					// dans le main ?
	//startThreadDetection(); 	// dans le main ?
}
Lidar::~Lidar(){

}
void Lidar::start(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_START;
	sendSPI(buffer,1);
}
void Lidar::stop(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_STOP;
	sendSPI(buffer,1);
}
void Lidar::getAvailableData(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_DATA_AVAILABLE;
	sendSPI(buffer,1);
}
void Lidar::getRawPoint(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_RAW_POINT;
	sendSPI(buffer,1);
}
void Lidar::sendGetDetectedPoints(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_DETECTED_POINTS;
	sendSPI(buffer,1);
}
void Lidar::setSpeed(uint8_t speed){	//change rotation speed
	uint8_t buffer[2];
	buffer[0] = LIDAR_CMD_SET_SPEED;
	buffer[1] = speed;
	sendSPI(buffer,2);
}
void Lidar::getSpeed(){	//change rotation speed
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_SPEED;
	sendSPI(buffer,1);
}

void Lidar::sendSPI(uint8_t *buf, uint8_t bufSize){	//add size & checksum
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
void Lidar::sendReceiveSPI(uint8_t data){	//send & handle response
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
void Lidar::flush(uint16_t nbBytes){
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
void Lidar::checkMessages(){
	while(nbMsgReceived > 0){
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
			std::cout << "CHECKSUM ERROR ! (msgSize = " << (int)msgSize << " & iRxOut = " << (int)iRxOut << ")" << std::endl;
		}
		else{	//Checksum ok
			switch(buf[1]){	//type of msg
				case LIDAR_RET_DEBUG_DEBUG:
					//std::cout << "Lidar> Debug : debug received" << std::endl;
					DEBUG_LIDAR_PRINT("Debug received");
					break;
				case LIDAR_RET_DEBUG_START:
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
						p.x = distance*cos(angle+3.14159);
						p.y = distance*sin(angle+3.14159);
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
			}
		}
	}
}
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
void Lidar::stopThreadDetection(){
	m_mutex.lock();
	m_continueThread = false;
	m_mutex.unlock();
}
void* thread_Lidar(void *threadid){
	Lidar *lidar = (Lidar*)threadid;

	while(lidar->isContinueThread()){
		lidar->sendGetDetectedPoints();
		lidar->flush(255);
		lidar->checkMessages();
		delay(1);

		//delay(10);
	}

	pthread_exit(NULL);
}

bool Lidar::isContinueThread(){
	m_mutex.lock();
	bool b = m_continueThread;
	m_mutex.unlock();
	return b;
}

void Lidar::addDetectedPoint(pointFloat2d p){
	m_mutex.lock();
	m_qDetectedPoints.push(p);
	m_mutex.unlock();
}
std::queue<pointFloat2d> Lidar::getDetectedPoints(){
	m_mutex.lock();
	std::queue<pointFloat2d> q = m_qDetectedPoints;
	m_mutex.unlock();
	return q;
}
std::queue<pointFloat2d> Lidar::getAndClearDetectedPoints(){
	m_mutex.lock();
	std::queue<pointFloat2d> q;
	std::swap(m_qDetectedPoints,q);
	m_mutex.unlock();
	return q;
}

void Lidar::setFillBuffer(bool b){
	m_mutex.lock();
	m_fillBuffer = b;
	m_mutex.unlock();
}
bool Lidar::getFillBuffer(){
	m_mutex.lock();
	bool b = m_fillBuffer;
	m_mutex.unlock();
	return b;
}
/*void Lidar::initPos(double x, double y, double t){
    setVarDouble64b(CODE_VAR_X_LD,x);
    setVarDouble64b(CODE_VAR_Y_LD,y);
    setVarDouble64b(CODE_VAR_T_LD,t);
}
void Lidar::setVarDouble64b(uint8_t varCode, double Var){
    double *ptrVar = &Var;
    uint8_t *ptr = (uint8_t*)ptrVar;
    uint8_t buffer[RX_SIZE_SET_64b + 1];
    buffer[0] = RX_SIZE_SET_64b;
    buffer[1] = RX_CODE_SET;
    buffer[2] = varCode;
    buffer[3] = VAR_LD_64b;
    buffer[4] = ptr[0];
    buffer[5] = ptr[1];
    buffer[6] = ptr[2];
    buffer[7] = ptr[3];
    buffer[8] = ptr[4];
    buffer[9] = ptr[5];
    buffer[10] = ptr[6];
    buffer[11] = ptr[7];
    buffer[12] = 0;
    for(int i = 0; i < RX_SIZE_SET_64b; i++){
        buffer[12] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_64b + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void Lidar::setVar32(uint8_t varCode, uint32_t var){

    uint8_t buffer[RX_SIZE_SET_32b + 1];
    buffer[0] = RX_SIZE_SET_32b;
    buffer[1] = RX_CODE_SET;
    buffer[2] = varCode;
    buffer[3] = VAR_32b;
    buffer[4] = (uint8_t)(var >> 24);
    buffer[5] = (uint8_t)(var >> 16);
    buffer[6] = (uint8_t)(var >> 8);
    buffer[7] = (uint8_t)(var & 0xFF);
    buffer[8] = 0;
    for(int i = 0; i < RX_SIZE_SET_32b; i++){
        buffer[8] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_32b + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void Lidar::setVar8(uint8_t varCode, uint8_t var){

    uint8_t buffer[RX_SIZE_SET_8b + 1];
    buffer[0] = RX_SIZE_SET_8b;
    buffer[1] = RX_CODE_SET;
    buffer[2] = varCode;
    buffer[3] = VAR_8b;
    buffer[4] = var;
    buffer[5] = 0;
    for(int i = 0; i < RX_SIZE_SET_8b; i++){
        buffer[5] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_8b + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void Lidar::getVar(uint8_t varCode){
    uint8_t buffer[RX_SIZE_GET + 1];
    buffer[0] = RX_SIZE_GET;
    buffer[1] = RX_CODE_GET;
    buffer[2] = varCode;
    buffer[3] = 0;
    for(int i = 0; i < RX_SIZE_GET; i++){
        buffer[3] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_GET + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
std::string Lidar::async_read(){
    std::string s("");
    while (serialDataAvail(fd)){
      s += serialGetchar(fd);
    }
    return s;
}*/