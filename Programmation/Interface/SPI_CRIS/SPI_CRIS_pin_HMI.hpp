#ifndef SPI_CRIS_PIN_H
#define SPI_CRIS_PIN_H

#define NB_SERVO        0
#define NB_MOTEUR4Q     0
#define NB_MOTEUR       0
#define NB_CAPT_CUR     0
#define NB_CAPT_COLOR   0
#define NB_UART         0
#define NB_RUPT         0
#define NB_AX12         0
#define NB_LIDAR        0
#define NB_SCREEN       1
#define NB_CAPT_DIST    0

#define SERVO_MIN     700
#define SERVO_MAX     1600

const int Pin_Led = 5;
#if NB_SERVO > 0
  const int Pin_Servo[] = {};
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
  #define PIN_ENABLE 1
  #if PIN_ENABLE > 0
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
  const int Pin_ax12_RX[NB_AX12];
  const int Pin_ax12_TX[NB_AX12];
#endif
#if NB_LIDAR > 0

#endif
#if NB_SCREEN > 0
  #define ADC0                A0
#endif
#if NB_CAPT_DIST > 0

#endif

#endif // SPI_CRIS_PIN_H