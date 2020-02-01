#include "actuator.hpp"

Actuator::Actuator(std::string nom, SPI *pSpi, uint8_t id, int nb_servo, int nb_moteur4Q, int nb_moteur, int nb_capt_cur, int nb_capt_couleur, int nb_uart, int nb_rupteur, int nb_ax12, int nb_capt_dist) 
: Robot(nom, pSpi, id) {
	m_nb_servo = nb_servo;
	m_nb_moteur4Q = nb_moteur4Q;
	m_nb_moteur = nb_moteur;
	m_nb_capt_cur = nb_capt_cur;
	m_nb_capt_couleur = nb_capt_couleur;
	m_nb_uart = nb_uart;
	m_nb_rupteur = nb_rupteur;
	m_nb_ax12 = nb_ax12;
	m_nb_capt_dist = nb_capt_dist;
    DEBUG_ROBOT_PRINT(m_nom << " comporte : ")
    DEBUG_ROBOT_PRINT("- " << m_nb_servo << " servo moteurs")
    DEBUG_ROBOT_PRINT("- " << m_nb_moteur4Q << " pont en H")
    DEBUG_ROBOT_PRINT("- " << m_nb_moteur << " moteurs")
    DEBUG_ROBOT_PRINT("- " << m_nb_capt_cur << " capteurs de courant")
    DEBUG_ROBOT_PRINT("- " << m_nb_capt_couleur << " capteurs de couleur")
    DEBUG_ROBOT_PRINT("- " << m_nb_uart << " uart")
    DEBUG_ROBOT_PRINT("- " << m_nb_rupteur << " rupteurs")
    DEBUG_ROBOT_PRINT("- " << m_nb_ax12 << " ax12")
    DEBUG_ROBOT_PRINT("- " << m_nb_capt_dist << " capteurs de distance")
}

Actuator::~Actuator() {}

void Actuator::checkMessages() {
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
				case ACT_CMD_CUR:
					m_cur[buf[2]] = buf[3];
					std::cout << buf[3] << std::endl;
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
				default:
					std::cout << "message non pris en charge" << std::endl;
					break;
			}
		}
	}
}

bool Actuator::startThreadDetection(){
	m_mutex.lock();
	m_continueThread = true;
	m_mutex.unlock();
	int rc = pthread_create(&m_thread, NULL, thread_act, this);
	if (rc) {
		std::cout << "Error:unable to create thread," << rc << std::endl;
		return false;
    }
	return true;
}

void* thread_act(void *threadid){
	Actuator *actuator = (Actuator*)threadid;
	while(actuator->isContinueThread()){
		actuator->checkMessages();
	}
	pthread_exit(NULL);
}

void Actuator::MoveServo(int nb_bras, int pos) {
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

void Actuator::SetMot(int nb_bras, int state) {
	uint8_t buffer[3];
	if(nb_bras < 0 || nb_bras > m_nb_moteur) {
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

void Actuator::SetMot4Q(int nb_bras, int vit, int sens) {
	uint8_t buffer[4];
	if(nb_bras < 0 || nb_bras > m_nb_moteur4Q) {
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

void Actuator::GetColor(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_capt_couleur) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_COLOR;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(5);
}

void Actuator::GetCurrent(int nb_bras){
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_capt_cur) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_CUR;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(5);
}

void Actuator::GetRupt(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_rupteur) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_RUPT;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(5);
}

void Actuator::GetDist(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_capt_dist) {
		std::cout << "erreur nb_bras = " << nb_bras << std::endl;
	} else {
		buffer[0] = ACT_CMD_DIST;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(5);
}

int Actuator::Cur(int nb_bras) {
	m_mutex.lock();
	int b = m_cur[nb_bras];
	m_mutex.unlock();
	return b;
}

int Actuator::Color(int nb_bras) {
	m_mutex.lock();
	int b = m_color[nb_bras];
	m_mutex.unlock();
	return b;
}

int Actuator::Dist(int nb_bras) {
	m_mutex.lock();
	int b = m_dist[nb_bras];
	m_mutex.unlock();
	return b;
}

int Actuator::Rupt(int nb_bras) {
	m_mutex.lock();
	int b = m_rupt[nb_bras];
	m_mutex.unlock();
	return b;
}

void Actuator::UartSend(unsigned char Send, unsigned char id_uart) {
	uint8_t buffer[5];
	buffer[0] = ACT_CMD_UART_SEND;
	buffer[1] = 3;
	buffer[2] = id_uart;
	buffer[3] = Send;
	buffer[4] = (3+id_uart+Send)%256;
	sendSPI(buffer,5);
}

void Actuator::allumerPhare(void) {
    UartSend(PHARE_CMD_ALLUMER,UART_ID_PHARE);
}

void Actuator::eteindrePhare(void) {
    UartSend(PHARE_CMD_ETEINDRE,UART_ID_PHARE);
}

void Actuator::setSeuilColor(int seuil, int valeur) {
	uint8_t buffer[4];
	if(seuil > 7 || seuil < 0) {
		std::cout << "erreur seuil " << seuil << std::endl;
	} else {
		buffer[0] = ACT_CMD_SEUIL_COLOR;
		buffer[1] = seuil;
		buffer[2] = (uint8_t)(valeur/256);
		buffer[3] = valeur%256;
		sendSPI(buffer,4);
	}
}

void Actuator::resetCptColor(void) {
	uint8_t buffer[1];
	buffer[0] = ACT_CMD_RESET_CPT_COLOR;
	sendSPI(buffer,1);
}