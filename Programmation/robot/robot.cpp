#include "robot.hpp"

Robot::Robot(std::string nom, SPI *pSpi,uint8_t id, int nb_servo, int nb_moteur4Q, int nb_moteur, int nb_capt_cur, int nb_capt_couleur, int nb_uart,
             int nb_rupteur, int nb_ax12, int nb_lidar, int nb_screen, int nb_capt_dist, Web *pWeb) {
    m_nom = nom;
    m_pSpi = pSpi;
	m_id = id;
    m_nb_servo = nb_servo;
    m_nb_moteur4Q = nb_moteur4Q;
    m_nb_moteurs = nb_moteur;
    m_nb_capt_cur = nb_capt_cur;
    m_nb_capt_couleur = nb_capt_couleur;
    m_nb_uart = nb_uart;
    m_nb_rupteur = nb_rupteur;
    m_nb_ax12 = nb_ax12;
    m_nb_lidar = nb_lidar;
    m_nb_screen = nb_screen;
    m_nb_capt_dist = nb_capt_dist;
    m_pWeb = pWeb;
    DEBUG_ROBOT_ENABLE_PRINT(m_nom+" comporte : ")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_nb_servo+" servo moteurs")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_nb_moteur4Q+" pont en H")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_nb_moteurs+" moteurs")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_nb_capt_cur+" capteurs de courant")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_nb_capt_couleur+" capteurs de courant")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_nb_uart+" capteurs de couleur")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_rupteur+" rupteurs")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_ax12+" ax12")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_lidar+" lidar")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_nb_screen+" écran")
    DEBUG_ROBOT_ENABLE_PRINT("- "+m_nb_capt_dist+" capteurs de distnace")
}

Robot::~Robot(){}

void Robot::checkMessages() {
	while(nbMsgReceived > 0) {
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
			std::cout << m_nom << " > CHECKSUM ERROR ! (msgSize = " << (int)msgSize << " & iRxOut = " << (int)iRxOut << ")" << " & CS = " << (int)buf[msgSize-1] << std::endl;
		} else {	//Checksum ok
			switch(buf[1]){	//type of msg
				case ACT_CMD_CUR:
					m_cur[buf[2]] = buf[3];
					break;
				case ACT_CMD_COLOR:
					m_color[buf[2]] = buf[3];
					break;
				case ACT_CMD_RUPT:
					m_rupt[buf[2]] = buf[3];
					break;
				case ACT_CMD_DIST:
					m_dist[buf[2]] = buf[3];
					break;
				case CMD_PING:
					if(buf[3] == 0x01) {
						m_ping = true;
					} else {
						m_ping = false;
					}
					break;
				case HMI_RET_COTE:
					m_hmi_cote = buf[3];
					break;
				case HMI_RET_OFF_PI:
					if(buf[3]) {
						m_stopPrgm = true;
						m_stopMain = true;
					}
					break;
				default:
					std::cout << "message non pris en charge" << std::endl;
					break;
			}
		}
	}
}

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

bool Robot::startThreadDetection(){
	m_mutex.lock();
	m_continueThread = true;
	m_mutex.unlock();
	int rc = pthread_create(&m_thread, NULL, thread, this);
	if (rc) {
		std::cout << "Error:unable to create thread," << rc << std::endl;
		return false;
    }
	return true;
}

void* thread(void *threadid){
	Robot *robot = (Robot*)threadid;
	while(robot->isContinueThread()){
		if(nb_lidar) {
			robot->sendGetDetectedPoints();
			robot->flush(255);
			robot->checkMessages();
			delay(1);
		} else if(nb_screen) {
			robot->flush(10);
			robot->checkMessages();
			delay(100);
		} else {
			robot->checkMessages();
		}
	}
	pthread_exit(NULL);
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
	buffer[0] = HMI_CMD_PING;
	sendSPI(buffer,1);
	flush(5);
}

bool Robot::Ping(void) {
	m_mutex.lock();
	bool b = m_ping;
	m_ping = flase;
	m_mutex.unlock();
	return b;
}

void Robot::MoveServo(int nb_bras, int pos) {
    uint8_t buffer[4];
	if(nb_bras < 0 || nb_bras > m_nb_servo) {
		std::cout << "erreur bras = " << nb_bras << std::endl;
	} else {
		if(pos < 600 || pos > 1600) {
			std::cout << "erreur pos = " << pos << std::endl;
		} else {
			buffer[0] = ACT_CMD_SERVO;
			buffer[1] = nb_bras;
			buffer[2] = (uint8_t)(pos/256);
			buffer[3] = pos%256;
			sendSPI(buffer,4);
		}
	}
}

void Robot::SetMot(int nb_bras, int state) {
	uint8_t buffer[3];
	if(nb_bras < 0 || nb_bras > nb_moteur) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		if(state < 0 || state > 1) {
			std::cout << "erreur state = " << state << std::endl;
		} else {
			buffer[0] = ACT_CMD_SET_MOT;
			buffer[1] = nb_bras;
			buffer[2] = state;
			sendSPI(buffer,3);
		}
	}
}

void Robot::SetMot4Q(int nb_bras, int vit, int sens) {
	uint8_t buffer[4];
	if(nb_bras < 0 || nb_bras > nb_moteur4Q) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		if(sens < 0 || sens > 1) {
			std::cout << "erreur sens = " << sens << std::endl;
		} else {
			if(vit < 0 || vit > 255) {
				std::cout << "erreur vit = " << vit << std::endl;
			} else {
				buffer[0] = ACT_CMD_SET_MOT4Q;
				buffer[1] = nb_bras;
				buffer[2] = sens;
				buffer[3] = vit;
				sendSPI(buffer,4);
			}
		}
	}
}

void Robot::GetColor(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > nb_capt_couleur) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_COLOR;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(5);
}

void Robot::GetCurrent(int nb_bras){
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > nb_capt_cur) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_CUR;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(5);
}

void Robot::GetRupt(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > nb_rupteur) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_RUPT;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(5);
}

void Robot::GetDist(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > nb_capt_dist) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = NB_CAPT_DIST;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(5);
}

int Robot::Cur(int nb_bras) {
	m_mutex.lock();
	int b = m_cur[nb_bras];
	m_mutex.unlock();
	return b;
}

int Robot::Color(int nb_bras) {
	m_mutex.lock();
	int b = m_color[nb_bras];
	m_mutex.unlock();
	return b;
}

int Robot::Dist(int nb_bras) {
	m_mutex.lock();
	int b = m_dist[nb_bras];
	m_mutex.unlock();
	return b;
}

int Robot::Rupt(int nb_bras) {
	m_mutex.lock();
	int b = m_rupt[nb_bras];
	m_mutex.unlock();
	return b;
}

void Robot::UartSend(std::string txt) {
	uint8_t buffer[txt.size()+3];
	char txt_char[txt.size()+1];
	int i;
	if(txt.size() < 20) {
		strcpy(txt_char, txt.c_str());
		buffer[0] = ACT_CMD_UART_SEND;
		for(i=0;i<txt.size()+1;i++) {
			buffer[i+1] = txt_char[i];
		}
		sendSPI(buffer,txt.size()+1);
	} else {
		std::cout << "erreur chaine trop longue max 20" << std::endl;
	}
}

void Robot::EraseScreen(int in_era) {
	uint8_t buffer[197];
	int i_era;
	buffer[0] = HMI_CMD_TXT;
	buffer[1] = in_era;
	for(i_era=0;i_era<195;i_era++) {
		buffer[i_era] = '\0';
	}
	sendSPI(buffer,197);
}

void Robot::SetPos(int pos_x, int pos_y) {
	uint8_t buffer[5];
	buffer[0] = HMI_CMD_POS;
	buffer[1] = (uint8_t)(pos_x/256);
	buffer[2] = pos_x%256;
	buffer[3] = (uint8_t)(pos_y/256);
	buffer[4] = pos_y%256;
	sendSPI(buffer,5);
}

void Robot::SetTxt(int in, std::string txt) {
	EraseScreen(in);
	delay(EraseTime);
	uint8_t buffer[txt.size()+3];
	char txt_char[txt.size()+1];
	int i;
	if(txt.size() < 196) {
		strcpy(txt_char, txt.c_str());
		buffer[0] = HMI_CMD_TXT;
		buffer[1] = in;
		for(i=0;i<txt.size()+1;i++) {
			buffer[i+2] = txt_char[i];
		}
		sendSPI(buffer,txt.size()+2);
	} else {
		std::cout << "erreur chaine trop longue max 195" << std::endl;
	}
}

void Robot::SetTxtFull(std::string txt_in) {
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

void Robot::setScore(int score) {
	uint8_t buffer[3];
	buffer[0] = HMI_CMD_SCORE;
	buffer[1] = score >> 8;
	buffer[2] = score & 0xFF;
	sendSPI(buffer,3);
}

bool Robot::isStopPrgm(void) {
	m_mutex.lock();
	bool b = m_stopPrgm;
	m_mutex.unlock();
	return b;
}

bool Robot::isStopMain(void) {
	m_mutex.lock();
	bool b = m_stopMain;
	m_mutex.unlock();
	return b;
}

uint8_t Robot::CoteChoisi(void) {
	m_mutex.lock();
	uint8_t b = m_hmi_cote;
	m_mutex.unlock();
	return b;
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
