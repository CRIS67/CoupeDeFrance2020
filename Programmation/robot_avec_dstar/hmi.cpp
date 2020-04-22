#include "hmi.hpp"

Hmi::Hmi(std::string nom, SPI *pSpi, uint8_t id) : Robot(nom, pSpi, id) {
	std::cout << std::endl;
}

Hmi::~Hmi() {}

void Hmi::DecodMsg(uint8_t buf[]) {
	switch(buf[1]){	//type of msg
		case HMI_RET_COTE:
			m_hmi_cote = buf[3];
			if(m_hmi_cote == JAUNE) {
    			DEBUG_ROBOT_PRINTLN("JAUNE SELECTED")
			} else if(m_hmi_cote == BLEU) {
    			DEBUG_ROBOT_PRINTLN("BLEU SELECTED")
			} else {
    			DEBUG_ROBOT_PRINTLN("AUCUN COTE")
			}
			break;
		case HMI_RET_OFF_PI:
			if(buf[3]) {
    			DEBUG_ROBOT_PRINTLN("turn off pi debug")
				m_stopPrgm = true;
				m_stopMain = true;
			}
			break;
		default:
    		DEBUG_ROBOT_PRINTLN("message non pris en charge")
			break;
	}
}

bool Hmi::startThreadDetection(){
	m_mutex.lock();
	m_continueThread = true;
	m_mutex.unlock();
	int rc = pthread_create(&m_thread, NULL, thread_Hmi, this);
	if (rc) {
    	DEBUG_ROBOT_PRINTLN("Error:unable to create thread," << rc)
		return false;
    }
	return true;
}

void* thread_Hmi(void *threadid){
	Hmi *hmi = (Hmi*)threadid;
	while(hmi->isContinueThread()){
		hmi->flush(10);
		hmi->checkMessages();
		delay(100);
	}
	pthread_exit(NULL);
}

void Hmi::EraseScreen(int in_era) {
	uint8_t buffer[197];
	int i_era;
	buffer[0] = HMI_CMD_TXT;
	buffer[1] = in_era;
	for(i_era=0;i_era<195;i_era++) {
		buffer[i_era] = '\0';
	}
	sendSPI(buffer,197);
}

void Hmi::SetPos(int pos_x, int pos_y) {
	uint8_t buffer[5];
	buffer[0] = HMI_CMD_POS;
	buffer[1] = (uint8_t)(pos_x/256);
	buffer[2] = pos_x%256;
	buffer[3] = (uint8_t)(pos_y/256);
	buffer[4] = pos_y%256;
	sendSPI(buffer,5);
}

void Hmi::SetTxt(int in, std::string txt) {
	EraseScreen(in);
	delay(EraseTime);
	uint8_t buffer[txt.size()+3];
	char txt_char[txt.size()+1];
	int i;
	if(txt.size() < 196) {
		strcpy(txt_char, txt.c_str());
		buffer[0] = HMI_CMD_TXT;
		buffer[1] = in;
		for(i=0;(unsigned int)i<txt.size()+1;i++) {
			buffer[i+2] = txt_char[i];
		}
		sendSPI(buffer,txt.size()+2);
	} else {
    	DEBUG_ROBOT_PRINTLN("erreur chaine trop longue max 195")
	}
}

void Hmi::SetTxtFull(std::string txt_in) {
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
    	DEBUG_ROBOT_PRINTLN("erreur chaine trop longue")
	}
}

void Hmi::setScore(int score) {
	uint8_t buffer[3];
	buffer[0] = HMI_CMD_SCORE;
	buffer[1] = score >> 8;
	buffer[2] = score & 0xFF;
	sendSPI(buffer,3);
}

bool Hmi::isStopPrgm(void) {
	m_mutex.lock();
	bool b = m_stopPrgm;
	m_mutex.unlock();
	return b;
}

bool Hmi::isStopMain(void) {
	m_mutex.lock();
	bool b = m_stopMain;
	m_mutex.unlock();
	return b;
}

uint8_t Hmi::CoteChoisi(void) {
	m_mutex.lock();
	uint8_t b = m_hmi_cote;
	m_mutex.unlock();
	return b;
}