#ifndef ACTUATIR_H
#define ACTUATIR_H

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <string.h>

#include "SPI.hpp"
#include "robot.hpp"

#define SERVO_VALUE_HIGH	700
#define SERVO_VALUE_MIDDLE	1000
#define SERVO_VALUE_DROP	1500
#define SERVO_VALUE_LOW		1600

#define UART_ID_PHARE		1
#define UART_ID_GR			2
#define UART_ID_HOLONOME	3
#define UART_ID_BALISE		4

#define LIM_RED_MIN   1
#define LIM_RED_MAX   2
#define LIM_GREEN_MIN 3
#define LIM_GREEN_MAX 4
#define LIM_BLUE_MIN  5
#define LIM_BLUE_MAX  6
#define LIM_WHITE_MAX 7

#define ERROR_VALUE		1025

void* thread_act(void *threadid);

class Actuator : public Robot {
	public:
		Actuator(std::string nom, SPI *pSpi, uint8_t id, int nb_servo, int nb_moteur4Q, int nb_moteur, int nb_capt_cur, int nb_capt_couleur, int nb_uart, int nb_rupteur, int nb_ax12, int nb_capt_dist);
		virtual ~Actuator();
		void DecodMsg(uint8_t buf[]);
		bool startThreadDetection();

		void MoveServo(int nb_bras, int pos);
		void SetMot(int nb_bras, int state);
		void SetAx12(int nb_bras, int pos);
		void GetColor(int nb_bras);
		void GetCurrent(int nb_bras);
		void GetRupt(int nb_bras);
		void GetDist(int nb_bras);
		int Cur(int nb_bras);
		int Color(int nb_bras);
		int ColorOne(int nb_bras);
		int Dist(int nb_bras);
		int Rupt(int nb_bras);
		int RuptOne(int nb_bras);
		void SetMot4QVit(int nb_bras, int vit, int sens);
		void SetMot4QPos(int nb_bras, int vit, int sens, int temps);
		void setSeuilColor(int seuil, int valeur);
		void resetCptColor();
		void UartSend(unsigned char Send, unsigned char id_uart, unsigned char msg);
		void allumerPhare();
		void eteindrePhare();
		bool GetPingXbee(uint8_t id);
		void PingXbee(uint8_t id);
		void resetXbee(uint8_t id);
	protected:
		int m_nb_servo;
        int m_nb_moteur4Q;
        int m_nb_moteur;
        int m_nb_capt_cur;
        int m_nb_capt_couleur;
        int m_nb_uart;
        int m_nb_rupteur;
        int m_nb_ax12;
        int m_nb_capt_dist;
        int m_cur[10] = {ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE};
        int m_color[10] = {ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE};
        int m_dist[10] = {ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE};
        int m_rupt[10] = {ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE, ERROR_VALUE};
        bool m_pingXbee[10] = {false, false, false, false, false, false, false, false, false, false};
		bool m_connectedXbee = false;
};

#endif // ACTUATIR_H