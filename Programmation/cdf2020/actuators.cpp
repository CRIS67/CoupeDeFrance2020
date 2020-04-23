#include "actuators.hpp"
int Send(int in){
	unsigned char buffer[1];
	buffer[0] = (unsigned char)in;
	wiringPiSPIDataRW(SPI_CHANNEL, buffer, 1);
	//std::cout << "entree = " << in << "   /   " << " reponse = " << (int)buffer[0] << std::endl;
	//delay(SPI_DELAY_MS);
	delayMicroseconds(SPI_DELAY_US);
	return (int)buffer[0];
	
	
}
Actuators::Actuators(SPI *pSpi,uint8_t id){
    m_pSpi = pSpi;
	m_id = id;
	
	uint8_t tempoIRxIn = iRxIn;
	
	GetCurrent(0);

	flush(255);

	//ajouter ping();
	if(tempoIRxIn == iRxIn) {
		std::cout << "actuators > non connecté" << std::endl;
		//ResetAtmega();
		//flush
		//if => cout
	}
}
Actuators::~Actuators(){

}
/************************************
*
* nom de la fonction : MoveServoExtr
*
* entrée : nb_bras, numéro du bras de 0 à 2
*
*          pos, position du bras extrême de 0 à 1 (tout en bas)
*
* tâche effectée : bouge le bras à la position indiquée
*
************************************/
void Actuators::MoveServoExtr(int nb_bras, int pos){
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		if(pos < 0 || pos > 1) {
			std::cout << "erreur pos = " << pos << std::endl;
		} else {
			buffer[0] = ACT_CMD_SERVO_EXT+nb_bras;
			buffer[1] = pos;
			sendSPI(buffer,2);
		}
	}
}
/************************************
*
* nom de la fonction : MoveServo
*
* entrée : nb_bras, numéro du bras de 0 à 2
*
*          pos, position du bras de 700 à 1600 (tout en bas)
*
* tâche effectée : bouge le bras à la position indiquée
*
************************************/
void Actuators::MoveServo(int nb_bras, int pos){
	uint8_t buffer[3];
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		if(pos < 600 || pos > 1600) {
			std::cout << "erreur pos = " << pos << std::endl;
		} else {
			buffer[0] = ACT_CMD_SERVO+nb_bras;
			buffer[1] = (uint8_t)(pos/256);
			buffer[2] = pos%256;
			sendSPI(buffer,3);
		}
	}
}
/************************************
* nom de la fonction : SetPump
*
* entrée : nb_bras, numéro du bras de 0 à 2
*
*          state, état du bras de 0 à 1 (allumée)
*
* tâche effectée : allume ou éteint la pompe
*
************************************/
void Actuators::SetPump(int nb_bras, int state){
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		if(state < 0 || state > 1) {
			std::cout << "erreur state = " << state << std::endl;
		} else {
			buffer[0] = ACT_CMD_PUMP+nb_bras;
			buffer[1] = state;
			sendSPI(buffer,2);
		}
	}
}
/************************************
* nom de la fonction : GetColor
*
* entrée : nb_bras, numéro du bras de 0 à 2
*
* tâche effectée : demande une lecture de la couleur
*
************************************/
void Actuators::GetColor(int nb_bras){
	uint8_t buffer[1];
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_COLOR+nb_bras;
		sendSPI(buffer,1);
	}
}
/************************************
* nom de la fonction : GetCurrent
*
* entrée : nb_bras, numéro du bras de 0 à 2
*
* tâche effectée : demande une mesure de courant
*
************************************/
void Actuators::GetCurrent(int nb_bras){
	uint8_t buffer[1];
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_CUR+nb_bras;
		sendSPI(buffer,1);
	}
}
/************************************
* nom de la fonction : GetCurrent
*
* entrée : nb_bras, numéro du bras de 0 à 2
*
* tâche effectée : demande une mesure de courant de la valeur
*
************************************/
void Actuators::GetCurrentFull(int nb_bras){
	uint8_t buffer[1];
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_CUR_FULL+nb_bras;
		sendSPI(buffer,1);
	}
}
/************************************
* nom de la fonction : Launchtest
*
* tâche effectée : teste un bras
*
************************************/
/*void Actuators::Launchtest() {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	int i = -1, res = 0;
	char in = 'a';
	std::cout << "numero du bras ? (0/1/2)" << std::endl;
	while(i < 0 || i > 2) {
		std::cin >> i;
	}
	MoveServoExtr(i, 0);
	delay(1000);
	std::cout << "placer un palet de couleur ? (R/G/B)" << std::endl;
	while(in != 'R' && in != 'G' && in != 'B') {
		std::cin >> in;
	}
	MoveServo(i,1600);
	delay(2000);
	SetPump(i, 1);
	delay(1000);
	GetCurrent(i);
	//get color
	sendReceiveSPI(2);
	sendReceiveSPI(i+10);
	res = sendReceiveSPI(12+i);
	if(res == 2) {
		std::cout << "pump succes" << std::endl;
	} else {
		if(res == 1) {
			std::cout << "pump empty" << std::endl;
		} else {
			std::cout << "pump error" << std::endl;
		}
	}
	//pump off
	sendReceiveSPI(3);
	sendReceiveSPI(i+1);
	sendReceiveSPI(0);
	res = sendReceiveSPI(4+i);
	if((res == 1 && in == 'R') || (res == 2 && in == 'G') || (res == 3 && in == 'B')) {
		std::cout << "succes" << std::endl;
	} else {
		std::cout << "fail" << std::endl;
	}
	MoveServoExtr(i, 0);
	m_pSpi->unlock();
}

int Actuators::debugGetCurrent(int nb_bras){
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	int ret = -1;
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras" << std::endl;
	} else {
		nb_bras += 13;
		sendReceiveSPI(2);
		sendReceiveSPI(nb_bras);
		sendReceiveSPI(2+nb_bras);
		unsigned char buffer[4];
		buffer[0] = sendReceiveSPI(0);
		buffer[1] = sendReceiveSPI(0);
		buffer[2] = sendReceiveSPI(0);
		buffer[3] = sendReceiveSPI(0);
		if(buffer[0] != 3){
			std::cout << "erreur fonction DebugGetCurrent : taille du message != 3" << std::endl;
		}
		if(buffer[1] != (nb_bras)){
			std::cout << "erreur fonction DebugGetCurrent : nb du bras" << std::endl;
		}
		uint8_t CS = buffer[0] + buffer[1] + buffer[2] + buffer[3];
		if(buffer[3] != CS){
			std::cout << "erreur fonction DebugGetCurrent : Checksum" << std::endl;
		}
		ret = buffer[2];
		std::cout << " debug current : " << (int)buffer[0] << " / " << (int)buffer[1] << " / " << (int)buffer[2] << " / " << (int)buffer[3] << std::endl;
	}
	m_pSpi->unlock();
	return ret;
}

int Actuators::debugGetCurrentFull(int nb_bras){
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	int ret = -1;
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras" << std::endl;
	} else {
		nb_bras += 16;
		sendReceiveSPI(2);
		sendReceiveSPI(nb_bras);
		sendReceiveSPI(2+nb_bras);
		
		unsigned char buffer[5];
		buffer[0] = sendReceiveSPI(0);
		buffer[1] = sendReceiveSPI(0);
		buffer[2] = sendReceiveSPI(0);
		buffer[3] = sendReceiveSPI(0);
		buffer[4] = sendReceiveSPI(0);
		if(buffer[0] != 4){
			std::cout << "erreur fonction DebugGetCurrent : taille du message != 3" << std::endl;
		}
		if(buffer[1] != (nb_bras)){
			std::cout << "erreur fonction DebugGetCurrent : nb du bras" << std::endl;
		}
		uint8_t CS = buffer[0] + buffer[1] + buffer[2] + buffer[3];
		if(buffer[4] != CS){
			std::cout << "erreur fonction DebugGetCurrent : Checksum" << std::endl;
		}
		ret = (buffer[2] << 8) + buffer[3];
		std::cout << " debug current : " << (int)buffer[0] << " / " << (int)buffer[1] << " / " << (int)buffer[2] << " / " << (int)buffer[3] << " / " << (int)buffer[4] << std::endl;
	}
	m_pSpi->unlock();
	return ret;
}
int Actuators::debugGetColor(int nb_bras){
	m_pSpi->lock();
	int ret = -1;
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}

	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras" << std::endl;
	} else {
		nb_bras += 10;
		sendReceiveSPI(2);
		sendReceiveSPI(nb_bras);
		sendReceiveSPI(2+nb_bras);
		unsigned char buffer[4];
		buffer[0] = sendReceiveSPI(0);
		buffer[1] = sendReceiveSPI(0);
		buffer[2] = sendReceiveSPI(0);
		buffer[3] = sendReceiveSPI(0);
		if(buffer[0] != 3){
			std::cout << "erreur fonction DebugGetCurrent : taille du message != 3" << std::endl;
		}
		if(buffer[1] != (nb_bras)){
			std::cout << "erreur fonction DebugGetCurrent : nb du bras" << std::endl;
		}
		uint8_t CS = buffer[0] + buffer[1] + buffer[2];
		if(buffer[3] != CS){
			std::cout << "erreur fonction DebugGetCurrent : Checksum" << std::endl;
		}
		std::cout << " debug color : " << (int)buffer[0] << " / " << (int)buffer[1] << " / " << (int)buffer[2] << " / " << (int)buffer[3] << std::endl;

	}
	m_pSpi->unlock();
	return ret;
}*/

void Actuators::ChangeColorLevel(int in, int val) {
	uint8_t buffer[3];
	if(in < 0 || in > 6) {
		std::cout << "Actuators::ChangeColorLevel>in range error = " << in << std::endl;
		return;
	}
	if(val < 0 || val > 900) {
		std::cout << "Actuators::ChangeColorLevel>val range error = " << val << std::endl;
		return;
	}
	buffer[0] = ACT_CMD_SEUIL_LEVEL+in;
	buffer[1] = (uint8_t)(val/256);
	buffer[2] = val%256;
	sendSPI(buffer,3);
}

void Actuators::GetColorFull(int in) {
	uint8_t buffer[1];
	while(in < 0 || in > 2) {
		std::cout << "Actuators::GetColorFull> in error range = " << in << std::endl; 
	}
	buffer[0] = ACT_CMD_COLOR_FULL+in;
	sendSPI(buffer,1);
}

void Actuators::ResetColorDefault(void) {
	uint8_t buffer[1];
	buffer[0] = ACT_CMD_SEUIL_RST;
	sendSPI(buffer,1);
}

void Actuators::ResetAtmega(void) {
	uint8_t buffer[1];
	buffer[0] = ACT_CMD_RST_UC;
	sendSPI(buffer,1);
}

void Actuators::checkMessages(){
	while(nbMsgReceived > 0){
		nbMsgReceived--;
		uint8_t msgSize = bufferRx[iRxOut];
		iRxOut++;

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

		checksum--; //err

		if(checksum != buf[msgSize-1]){
			std::cout << "Actuators > CHECKSUM ERROR ! (msgSize = " << (int)msgSize << " & iRxOut = " << (int)iRxOut << ")" << " & CS = " << (int)buf[msgSize-1] << std::endl;
		} else {	//Checksum ok
			std::cout << "msg receive" << std::endl;
			switch(buf[1]){	//type of msg
				std::cout << "Actuators> color 1" << buf[2] << std::endl;
				case ACT_RET_COLOR_1:
					std::cout << "Actuators> color 1" << buf[2] << std::endl;
					break;
				/*case HMI_SHUTDOWN_PI:
					//std::cout << "Lidar> Debug : debug received" << std::endl;
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

void Actuators::sendSPI(uint8_t *buf, uint8_t bufSize){	//add size & checksum
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

void Actuators::sendReceiveSPI(uint8_t data){	//send & handle response
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

void Actuators::flush(uint16_t nbBytes){
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

bool Actuators::flushDyn(void) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	uint8_t buffer[1];
	uint8_t tempoIRxIn = iRxIn, cptRx = 0;
	bool e = false;
	for(uint16_t i = 0; i < 255; i++){
		buffer[0] = 0;
		sendReceiveSPI(buffer[0]);
		delayMicroseconds(SPI_DELAY_US);
		delay(10);
		if(tempoIRxIn == iRxIn) {
			cptRx++;
		}
		if(cptRx > NB_FLUSH_MIN) {
			e = true;
			break;
		}
	}
	m_pSpi->unlock();
	return e;
}

bool Actuators::startThreadDetection(){
	m_mutex.lock();
	m_continueThread = true;
	m_mutex.unlock();
	int rc = pthread_create(&m_thread, NULL, thread_Actuators, this);
	if (rc) {
		std::cout << "Error:unable to create thread," << rc << std::endl;
		return false;
    }
	return true;
}

void Actuators::stopThreadDetection(){
	m_mutex.lock();
	m_continueThread = false;
	m_mutex.unlock();
}

void* thread_Actuators(void *threadid){
	Actuators *actuators = (Actuators*)threadid;
	uint16_t CptFlush = 1;
	bool c = false;

	while(actuators->isContinueThread()){
		c = actuators->flushDyn();
		if(c == true) {
			if(CptFlush < TEMP_MAX_FLUSH) {
				CptFlush = CptFlush*2;
			}
		} else {
			CptFlush = 1;
		}
		actuators->checkMessages();
		delay(CptFlush);
	}
	
	pthread_exit(NULL);
}

bool Actuators::isContinueThread(){
	m_mutex.lock();
	bool b = m_continueThread;
	m_mutex.unlock();
	return b;
}