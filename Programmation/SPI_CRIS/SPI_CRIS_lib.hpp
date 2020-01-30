#ifndef SPI_CRIS_LIB_H
#define SPI_CRIS_LIB_H

#include <Servo.h>
#include <SoftwareSerial.h>
#include <Arduino.h>

//à modifier
#define NB_SERVO        3
#define NB_MOTEUR       3
#define NB_MOTEUR4Q     0
#define NB_CAPT_CUR     3
#define NB_CAPT_COLOR   3
#define NB_UART         0
#define NB_RUPT         0
#define NB_AX12         0
#define NB_LIDAR        0
#define NB_SCREEN       0
#define NB_CAPT_DIST    0

#define SERVO_MIN     700
#define SERVO_MAX     1600

const int Pin_Led;
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

#endif
#if NB_UART > 0
  
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

#endif
#if NB_CAPT_DIST > 0

#endif

//ne pas toucher
void InitCrisSpi();
void LoopCrisSpi();
void ISRCrisSpi();

//etat machine spi
#define SPI_IDLE        0
#define SPI_NUM_VAR     1
#define SPI_MSG_UTILE   2
#define SPI_CHECKSUM    3

#define TAILLE_SPI_CHAINE   100
#define TAILLE_SEND         10

#define AUCUN                           0
#define LIDAR_CMD_DEBUG                 1
#define LIDAR_CMD_START                 2
#define LIDAR_CMD_STOP                  3
#define LIDAR_CMD_GET_DISTANCE          4
#define LIDAR_CMD_GET_ANGLE             5
#define LIDAR_CMD_GET_STARTBIT          6
#define LIDAR_CMD_GET_QUALITY           7
#define LIDAR_CMD_SET_AUTOSTART         8
#define LIDAR_CMD_RESET_AUTOSTART       9
#define LIDAR_CMD_GET_DATA_AVAILABLE    10
#define LIDAR_CMD_GET_DETECTED_POINTS   11
#define LIDAR_CMD_GET_RAW_POINT         12
#define LIDAR_CMD_SET_SPEED             20
#define LIDAR_CMD_GET_SPEED             21
#define ACT_CMD_SERVO                   22
#define ACT_CMD_SET_MOT4Q               23
#define ACT_CMD_SET_MOT                 25
#define ACT_CMD_CUR                     26
#define ACT_CMD_COLOR                   27
#define ACT_CMD_UART_SEND               28
#define ACT_CMD_RUPT                    29
#define ACT_CMD_AX12                    30
#define ACT_CMD_DIST                    31
#define CMD_RST                         32
#define CMD_PING                        33
#define HMI_CMD_POS                     34
#define HMI_CMD_TXT                     35
#define HMI_CMD_SCORE                   36
#define HMI_RET_COTE                    37
#define HMI_RET_OFF_PI                  38
#define LIDAR_RET_DEBUG_DEBUG           42 
#define LIDAR_RET_DEBUG_START           43 
#define LIDAR_RET_DEBUG_STOP            44
#define LIDAR_RET_DATA_AVAILABLE        100
#define LIDAR_RET_DETECTED_POINTS       101
#define LIDAR_RET_RAW_POINT             102
#define LIDAR_RET_SPEED                 121

#define MOTEUR_STOP     0

#endif //SPI_CRIS_LIB_H
