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
	std::cout << m_nom << " comporte : " << std::endl;
	if(m_nb_servo) {std::cout << m_nb_servo << " servo moteurs" << std::endl;}
	if(m_nb_moteur4Q) {std::cout << m_nb_moteur4Q << " pont en H" << std::endl;}
	if(m_nb_moteur) {std::cout << m_nb_moteur << " moteurs" << std::endl;}
	if(m_nb_capt_cur) {std::cout << m_nb_capt_cur << " capteurs de courant" << std::endl;}
	if(m_nb_capt_couleur) {std::cout << m_nb_capt_couleur << " capteurs de couleur" << std::endl;}
	if(m_nb_uart) {std::cout << m_nb_uart << " uart" << std::endl;}
	if(m_nb_rupteur) {std::cout << m_nb_rupteur << " rupteurs" << std::endl;}
	if(m_nb_ax12) {std::cout << m_nb_ax12 << " ax12" << std::endl;}
	if(m_nb_capt_dist) {std::cout << m_nb_capt_dist << " capteurs de distance" << std::endl;}
	std::cout << std::endl;
}

Actuator::~Actuator() {}

void Actuator::DecodMsg(uint8_t buf[]) {
	switch(buf[1]){	//type of msg
		case ACT_CMD_CUR:
			m_cur[buf[2]] = buf[3]*256+buf[4];
			break;
		case ACT_CMD_COLOR:
			m_color[buf[2]] = buf[3]*256+buf[4];
			std::cout << m_color[buf[2]] << std::endl;
			break;
		case ACT_CMD_RUPT:
			m_rupt[buf[2]] = buf[3];
			break;
		case ACT_CMD_DIST:
			m_dist[buf[2]] = buf[3]*256+buf[4];
			break;
		case ACT_CMD_UART_SEND:
			if(buf[6] == (buf[2]+buf[3]+buf[4]+buf[5])%256) { //test CS
				if(buf[2] == 5) {
					if(buf[3] == UART_ID_PHARE) {
						if(buf[4] == PHARE_CMD_ALLUMER) {
							m_phareAllumee = buf[5];
						} else if(buf[4] == PHARE_CMD_ETEINDRE) {
							m_phareEteint = buf[5];
						} else {
    						DEBUG_ROBOT_PRINTLN("erreur type message")
						}
					} else {
    					DEBUG_ROBOT_PRINTLN("wrong ID UART")
					}
				} else {
    				DEBUG_ROBOT_PRINTLN("wrong length UART")
				}
			} else {
    			DEBUG_ROBOT_PRINTLN("wrong CS UART")
			}
			break;
		default:
    		DEBUG_ROBOT_PRINTLN("message non pris en charge")
			break;
	}
}

bool Actuator::startThreadDetection(){
	m_mutex.lock();
	m_continueThread = true;
	m_mutex.unlock();
	int rc = pthread_create(&m_thread, NULL, thread_act, this);
	if (rc) {
    	DEBUG_ROBOT_PRINTLN("Error:unable to create thread," << rc)
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
    	DEBUG_ROBOT_PRINTLN("erreur bras = " << nb_bras)
	} else {
		if(pos < 600 || pos > 1600) {
    	DEBUG_ROBOT_PRINTLN("erreur pos = " << pos)
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
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		if(state < 0 || state > 1) {
    	DEBUG_ROBOT_PRINTLN("erreur state = " << state)
		} else {
			buffer[0] = ACT_CMD_SET_MOT;
			buffer[1] = nb_bras;
			buffer[2] = state;
			sendSPI(buffer,3);
		}
	}
}

void Actuator::SetMot4QVit(int nb_bras, int vit, int sens) {
	uint8_t buffer[4];
	if(nb_bras < 0 || nb_bras > m_nb_moteur4Q) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		if(sens < 0 || sens > 1) {
    		DEBUG_ROBOT_PRINTLN("erreur sens = " << sens)
		} else {
			if(vit < 0 || vit > 255) {
    			DEBUG_ROBOT_PRINTLN("erreur vit = " << vit)
			} else {
				buffer[0] = ACT_CMD_SET_MOT4QVit;
				buffer[1] = nb_bras;
				buffer[2] = sens;
				buffer[3] = vit;
				sendSPI(buffer,4);
			}
		}
	}
}

void Actuator::SetAx12(int nb_bras, int pos) {
	uint8_t buffer[4];
	if(nb_bras < 1 || nb_bras > m_nb_ax12+1) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		if(pos < 0 || pos > 1000) {
    		DEBUG_ROBOT_PRINTLN("erreur pos = " << pos)
		} else {
			buffer[0] = ACT_CMD_AX12;
			buffer[1] = nb_bras;
			buffer[2] = (uint8_t)(pos/256);
			buffer[3] = pos%256;
			sendSPI(buffer,4);
		}
	}
}

void Actuator::GetColor(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_capt_couleur) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		buffer[0] = ACT_CMD_COLOR;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(NB_FLUSH_MIN);
}

void Actuator::GetCurrent(int nb_bras){
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_capt_cur) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		buffer[0] = ACT_CMD_CUR;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(NB_FLUSH_MIN);
}

void Actuator::GetRupt(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_rupteur) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		buffer[0] = ACT_CMD_RUPT;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(NB_FLUSH_MIN);
}

void Actuator::GetDist(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_capt_dist) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		buffer[0] = ACT_CMD_DIST;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(NB_FLUSH_MIN);
}

int Actuator::Cur(int nb_bras) {
	m_mutex.lock();
	int cpt = 0;
	do{
		GetCurrent(nb_bras);
		delay(20);
		cpt++;
	}while(m_cur[nb_bras] == ERROR_VALUE && cpt < 10);
	if(cpt > 1) {DEBUG_ROBOT_PRINTLN("plusieur essais " << cpt)}
	if(m_cur[nb_bras] == ERROR_VALUE) {DEBUG_ROBOT_PRINTLN("ERROR courant")}
	int b = m_cur[nb_bras];
	m_cur[nb_bras] = ERROR_VALUE;
	m_mutex.unlock();
	return b;
}

int Actuator::Color(int nb_bras) {
	m_mutex.lock();
	int cpt = 0;
	do{
		GetColor(nb_bras);
		delay(20);
		cpt++;
	}while(m_color[nb_bras] == ERROR_VALUE && cpt < 10);
	if(cpt > 1) {DEBUG_ROBOT_PRINTLN("plusieur essais " << cpt)}
	if(m_color[nb_bras] == ERROR_VALUE) {DEBUG_ROBOT_PRINTLN("ERROR couleur")}
	int b = m_color[nb_bras];
	m_color[nb_bras] = ERROR_VALUE;
	m_mutex.unlock();
	return b;
}

int Actuator::Dist(int nb_bras) {
	m_mutex.lock();
	int cpt = 0;
	do{
		GetDist(nb_bras);
		delay(20);
		cpt++;
	}while(m_dist[nb_bras] == ERROR_VALUE && cpt < 10);
	if(cpt > 1) {DEBUG_ROBOT_PRINTLN("plusieur essais " << cpt)}
	if(m_dist[nb_bras] == ERROR_VALUE) {DEBUG_ROBOT_PRINTLN("ERROR couleur")}
	int b = m_dist[nb_bras];
	m_dist[nb_bras] = ERROR_VALUE;
	m_mutex.unlock();
	return b;
}

int Actuator::Rupt(int nb_bras) {
	m_mutex.lock();
	int cpt = 0;
	do{
		GetRupt(nb_bras);
		delay(20);
		cpt++;
	}while(m_rupt[nb_bras] == ERROR_VALUE && cpt < 10);
	if(cpt > 1) {DEBUG_ROBOT_PRINTLN("plusieur essais " << cpt)}
	if(m_rupt[nb_bras] == ERROR_VALUE) {DEBUG_ROBOT_PRINTLN("ERROR couleur")}
	int b = m_rupt[nb_bras];
	m_rupt[nb_bras] = ERROR_VALUE;
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
    	DEBUG_ROBOT_PRINTLN("erreur seuil " << seuil)
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

bool Actuator::isPhareAllumee(void) {
	m_mutex.lock();
	int b = m_phareAllumee;
	m_phareAllumee = 0;
	m_mutex.unlock();
	return b;
}

bool Actuator::isPhareEteint(void) {
	m_mutex.lock();
	int b = m_phareEteint;
	m_phareEteint = 0;
	m_mutex.unlock();
	return b;
}