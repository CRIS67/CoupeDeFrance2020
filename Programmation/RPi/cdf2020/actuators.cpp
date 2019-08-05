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
	
	flush(255);
	//ajouter ping();
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
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras" << std::endl;
	} else {
		if(pos < 0 || pos > 1) {
			std::cout << "erreur pos" << std::endl;
		} else {
			nb_bras += 4;
			Send(3);
			Send(nb_bras);
			Send(pos);
			Send(3+nb_bras+pos);
		}
	}
	m_pSpi->unlock();
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
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras" << std::endl;
	} else {
		if(pos < 600 || pos > 1600) {
			std::cout << "erreur pos" << std::endl;
		} else {
			nb_bras += 7;
			int lsb = pos%256, msb = (int)(pos/256);
			Send(4);
			Send(nb_bras);
			Send(msb);
			Send(lsb);
			Send(4+nb_bras+lsb+msb);
		}
	}
	m_pSpi->unlock();
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
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras" << std::endl;
	} else {
		if(state < 0 || state > 1) {
			std::cout << "erreur state" << std::endl;
		} else {
			nb_bras ++;
			Send(3);
			Send(nb_bras);
			Send(state);
			Send(3+nb_bras+state);
		}
	}
	m_pSpi->unlock();
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
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras" << std::endl;
	} else {
		nb_bras += 10;
		Send(2);
		Send(nb_bras);
		Send(2+nb_bras);
	}
	m_pSpi->unlock();
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
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras" << std::endl;
	} else {
		nb_bras += 13;
		Send(2);
		Send(nb_bras);
		Send(2+nb_bras);
	}
	m_pSpi->unlock();
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
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	if(nb_bras < 0 || nb_bras > 2) {
		std::cout << "erreur nb_bras" << std::endl;
	} else {
		nb_bras += 16;
		Send(2);
		Send(nb_bras);
		Send(2+nb_bras);
	}
	m_pSpi->unlock();
}
/************************************
* nom de la fonction : Launchtest
*
* tâche effectée : teste un bras
*
************************************/
void Actuators::Launchtest() {
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
	Send(2);
	Send(i+10);
	res = Send(12+i);
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
	Send(3);
	Send(i+1);
	Send(0);
	res = Send(4+i);
	if((res == 1 && in == 'R') || (res == 2 && in == 'G') || (res == 3 && in == 'B')) {
		std::cout << "succes" << std::endl;
	} else {
		std::cout << "fail" << std::endl;
	}
	MoveServoExtr(i, 0);
	m_pSpi->unlock();
}
/************************************
* nom de la fonction : flush
*
* tâche effectée : envoie "nb" octets 0 à la carte
*
************************************/
void Actuators::flush(uint16_t nb){
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	for(uint16_t i = 0; i < nb; i++){
		Send(0);
	}
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
		Send(2);
		Send(nb_bras);
		Send(2+nb_bras);
		unsigned char buffer[4];
		buffer[0] = Send(0);
		buffer[1] = Send(0);
		buffer[2] = Send(0);
		buffer[3] = Send(0);
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
		Send(2);
		Send(nb_bras);
		Send(2+nb_bras);
		
		unsigned char buffer[5];
		buffer[0] = Send(0);
		buffer[1] = Send(0);
		buffer[2] = Send(0);
		buffer[3] = Send(0);
		buffer[4] = Send(0);
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
		Send(2);
		Send(nb_bras);
		Send(2+nb_bras);
		unsigned char buffer[4];
		buffer[0] = Send(0);
		buffer[1] = Send(0);
		buffer[2] = Send(0);
		buffer[3] = Send(0);
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
}
void Actuators::ChangeColorLevel(int in, int val) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	if(in < 0 || in > 6) {
		std::cout << "Actuators::ChangeColorLevel>in range error" << std::endl;
		return;
	}
	if(val < 0 || val > 900) {
		std::cout << "Actuators::ChangeColorLevel>val range error" << std::endl;
		return;
	}
	int lsb = val%256, msb = int(val/256);
	in += 22;
	Send(4);
	Send(in);
	Send(msb);
	Send(lsb);
	Send(in+4+lsb+msb);
	m_pSpi->unlock();
}
uint16_t Actuators::GetColorFull(int in) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	while(in < 0 || in > 2) {
		std::cout << "Actuators::GetColorFull> in error range" << std::endl; 
		return 0;
	}
	in += 19;
	uint8_t buffer[5];
	Send(2);
	Send(in);
	Send(2+in);
	buffer [0] = Send(0);
	buffer [1] = Send(0);
	buffer [2] = Send(0);
	buffer [3] = Send(0);
	buffer [4] = Send(0);
	m_pSpi->unlock();
	uint16_t val = (buffer[2] << 8) + buffer[3];
	return val;
}
void Actuators::ResetColorDefault(void) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	Send(2);
	Send(29);
	Send(31);
	m_pSpi->unlock();
}
void Actuators::ResetAtmega(void) {
	m_pSpi->lock();
	if(m_pSpi->getSlaveId() != m_id){	
		m_pSpi->setSlave(m_id);		//change Chip select
	}
	Send(2);
	Send(30);
	Send(32);
	m_pSpi->unlock();
}