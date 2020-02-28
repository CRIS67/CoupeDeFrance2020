#ifndef SPI_CRIS_PIN_H
#define SPI_CRIS_PIN_H

#define COM_SPI
//#define COM_UART

#define NB_SERVO        6
#define NB_MOTEUR4Q     0
#define NB_MOTEUR       0
#define NB_CAPT_CUR     0
#define NB_CAPT_COLOR   2
#define NB_UART         0
#define NB_RUPT         6
#define NB_AX12         0
#define NB_LIDAR        0
#define NB_SCREEN       0
#define NB_CAPT_DIST    0

#define SERVO_MIN     700
#define SERVO_MAX     1600

const int Pin_Led = 9;
#ifdef COM_UART
  #define BAUDRATE_UART   9600
  #define ADDR_SLAVE      UART_ID_PHARE
#endif
#if NB_SERVO > 0
  const int Pin_Servo[] = {3, 4, 5, 6, 7, 7, 8};
#endif
#if NB_MOTEUR > 0
  const int Pin_Moteur[] = {};
#endif
#if NB_MOTEUR4Q > 0
  const int Pin_Moteur4Q_SENS[] = {};
  const int Pin_Moteur4Q_PWM[] = {};
#endif
#if NB_CAPT_CUR > 0
  const int Pin_Capt_Cur[] = {};
#endif
#if NB_CAPT_COLOR > 0
  #define PIN_ENABLE
  #ifdef PIN_ENABLE
  	const int Pin_Capt_Color_Ena = 0;
  #endif
  const int Pin_Capt_Color_TS2 = 1;
  const int Pin_Capt_Color_TS3 = 2;
  const int Pin_Capt_Color_Arm[NB_CAPT_COLOR] = {A6, A7};
#endif
#if NB_RUPT > 0
  const int Pin_Rupt[] = {A0, A1, A2, A3, A4, A5};
#endif
#if NB_AX12 > 0
  const int IdAx12[NB_AX12] = {1, 2};
  const int PinDir = 2;
#endif
#if NB_LIDAR > 0

#endif
#if NB_SCREEN > 0
  #define ADC0                A0
#endif
#if NB_CAPT_DIST > 0

#endif

#endif // SPI_CRIS_PIN_H
