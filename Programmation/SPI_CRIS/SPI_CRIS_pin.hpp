#ifndef SPI_CRIS_PIN_H
#define SPI_CRIS_PIN_H

#define COM_SPI
//#define COM_UART

#define NB_SERVO        3
#define NB_MOTEUR4Q     0
#define NB_MOTEUR       3
#define NB_CAPT_CUR     3
#define NB_CAPT_COLOR   0
#define NB_UART         0
#define NB_RUPT         0
#define NB_AX12         2
#define NB_LIDAR        0
#define NB_SCREEN       0
#define NB_CAPT_DIST    0

#define SERVO_MIN     700
#define SERVO_MAX     1600

const int Pin_Led = 8;
#ifdef COM_UART
  #define BAUDRATE_UART   9600
  #define ADDR_SLAVE      UART_ID_PHARE
#endif
#if NB_SERVO > 0
  const int Pin_Servo[] = {2, 3, 4};
#endif
#if NB_MOTEUR > 0
  const int Pin_Moteur[] = {5, 6, 7};
#endif
#if NB_MOTEUR4Q > 0
  const int Pin_Moteur4Q_SENS[] = {};
  const int Pin_Moteur4Q_PWM[] = {};
  const int Pin_RuptEnd4Q[][2] = {{7, A0},{A1, A2}};
#endif
#if NB_CAPT_CUR > 0
  const int Pin_Capt_Cur[] = {A5, A6, A7};
#endif
#if NB_CAPT_COLOR > 0
  #define PIN_ENABLE
  #ifdef PIN_ENABLE
  	const int Pin_Capt_Color_Ena;
  #endif
  const int Pin_Capt_Color_TS2[NB_CAPT_COLOR];
  const int Pin_Capt_Color_TS3[NB_CAPT_COLOR];
  const int Pin_Capt_Color_Arm[NB_CAPT_COLOR];
#endif
#if NB_RUPT > 0
  const int Pin_Rupt[] = {};
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
#if NB_UART > 0
  const int XBee_Adress = 1;
  const int Other_Adress = 2;
  const String Network_Adress = "1234";
#endif
#ifdef COM_UART
  const int LED_PHARE = 8;
#endif

#endif // SPI_CRIS_PIN_H
