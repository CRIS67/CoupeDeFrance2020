#ifndef ACTUATIR_H
#define ACTUATIR_H

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <cmath>

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

#define D1  80
#define D2  60
#define PI  3.14159265358979
#define PTxFORB  100
#define PTyFORB  100

#define ZONE_1 1
#define ZONE_2 0

#define ERROR_VALUE		1025

void* thread_act(void *threadid);

class Actuator : public Robot {
	public:
		Actuator();
		Actuator(const Actuator&);
		Actuator(std::string nom, SPI *pSpi, uint8_t id, int nb_servo, int nb_moteur4Q, int nb_moteur, int nb_capt_cur, int nb_capt_couleur, int nb_rupteur, int nb_ax12, int nb_capt_dist);
    	Actuator(std::string nom, SPI *pSpi, uint8_t id, int nb_servo, int nb_moteur4Q, int nb_moteur, int nb_capt_cur, int nb_capt_couleur, int nb_rupteur, int nb_ax12, int nb_capt_dist, int nb_uart, std::string uart_name[], int uart_addr[]);
		virtual ~Actuator();
		void DecodMsg(uint8_t buf[]);
		bool startThreadDetection();

		bool MoveServo(int nb_bras, int pos);
		bool SetMot(int nb_bras, int state);
		bool SetAx12(int nb_bras, int pos);
		bool MoveAx12(int nb_bras, int d, int a);
		void CalculerPosScara(double d, double thet, int* thetI1, int* thetI2);
		uint8_t TestScara(int d, int thet, int* thetI1, int* thetI2);
		bool GoScara(int thet, int thet1, int thet2);
		void GetColor(int nb_bras);
		void GetCurrent(int nb_bras);
		void GetRupt(int nb_bras);
		void GetDist(int nb_bras);
		int Cur(int nb_bras);
		int Color(int nb_bras);
		int Dist(int nb_bras);
		int Rupt(int nb_bras);
		bool SetMot4QVit(int nb_bras, int vit, int sens);
		bool SetMot4QPos(int nb_bras, int vit, int sens, int temps);
		bool SetMot4QPosNorm(int nb_bras, int pos);
		void setSeuilColor(int seuil, int valeur);
		void resetCptColor();
		void UartSend(unsigned char Send, unsigned char id_uart, unsigned char msg);
		void allumerPhare();
		void eteindrePhare();
		bool GetPingXbee();
		void PingXbee(uint8_t id);
		void resetXbee(uint8_t id);
		bool GetAck();
	protected:
		uint8_t Zone = ZONE_1;
		bool m_ack = false;
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
        bool m_pingXbee = false;
		    bool m_connected_uart[10] = {false, false, false, false, false, false, false, false, false, false};
        std::string m_uart_name[10] = {"","","","","","","","","",""};
        int m_uart_addr[10] = {0,0,0,0,0,0,0,0,0,0};
};

#endif // ACTUATIR_H