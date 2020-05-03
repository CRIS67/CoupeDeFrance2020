#ifndef SPI_CRIS_PIN_H
#define SPI_CRIS_PIN_H

#define COM_SPI
//#define COM_UART

#define NB_SERVO        4
#define NB_MOTEUR4Q     2
#define NB_MOTEUR       0
#define NB_CAPT_CUR     0
#define NB_CAPT_COLOR   0
#define NB_UART         1
#define NB_RUPT         3
#define NB_AX12         0
#define NB_SCREEN       0
#define NB_CAPT_DIST    0

const int Pin_Led = 2;
#ifdef COM_UART
  const int LED_PHARE = ;
#endif
#if NB_SERVO > 0
  #define SERVO_MIN     700
  #define SERVO_MAX     1600
  const int Pin_Servo[] = {7, 8, 9, A0};
#endif
#if NB_MOTEUR4Q > 0
  const int Pin_Moteur4Q_SENS[] = {3,4};
  const int Pin_Moteur4Q_PWM[] = {5, 6};
  const int Pin_RuptEnd4Q[][2] = {{A1, A2},{A3, A3}};
#endif
#if NB_MOTEUR > 0
  const int Pin_Moteur[] = {};
#endif
#if NB_CAPT_CUR > 0
  const int Pin_Capt_Cur[] = {};
#endif
#if NB_CAPT_COLOR > 0
  #define PIN_ENABLE 1
  #if PIN_ENABLE > 0
  	const int Pin_Capt_Color_Ena;
  #endif
  const int Pin_Capt_Color_TS2 = ;
  const int Pin_Capt_Color_TS3 = ;
  const int Pin_Capt_Color_Arm[NB_CAPT_COLOR];
#endif
#if NB_UART > 0
  const int XBee_Adress = 2;
  const int Other_Adress = 1;
  const String Network_Adress = "1234";
#endif
#if NB_RUPT > 0
  const int Pin_Rupt[] = {A1, A2, A3};
#endif
#if NB_AX12 > 0
  const int PinDir = ;
#endif
#if NB_SCREEN > 0
  #define ADC0                A0
#endif
#if NB_CAPT_DIST > 0

#endif

#endif // SPI_CRIS_PIN_H
