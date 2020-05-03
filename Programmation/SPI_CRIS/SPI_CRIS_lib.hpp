#ifndef SPI_CRIS_LIB_H
#define SPI_CRIS_LIB_H

#include <Servo.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include "SPI_CRIS_pin_Xbee.hpp"
//#include "SPI_CRIS_pin_Phare.hpp"
//#include "SPI_CRIS_pin_Act.hpp"
//#include "SPI_CRIS_pin_Scara.hpp"
//#include "SPI_CRIS_pin_HMI.hpp"
//#include "SPI_CRIS_pin.hpp"
#include "projet.h"

void InitCrisSpi();
void LoopCrisSpi();
unsigned char ISRCrisSpi(unsigned char data_spi);
void SendSpi(uint8_t buf, uint8_t leng, uint8_t num);

#define SS              10
#define MOSI            11
#define MISO            12
#define SCK             13

#define UART_ID_PHARE     1
#define UART_ID_HOLONOME  2
#define UART_ID_BALISE    3

//etat machine spi
#define SPI_IDLE        0
#define SPI_NUM_VAR     1
#define SPI_MSG_UTILE   2
#define SPI_CHECKSUM    3
#define SPI_ADDR        4

#define SPI_SEND_IDLE   0
#define SPI_SEND_TYPE   1
#define SPI_SEND_NUM    2
#define SPI_SEND_MSG    3
#define SPI_SEND_MSG2   4
#define SPI_SEND_CHECK  5

#define TAILLE_SPI_CHAINE   100
#define TAILLE_SEND         10

//------------ CMD -------------------//
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
#define ACT_CMD_SET_MOT4QVit            23
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
#define ACT_CMD_SEUIL_COLOR             39
#define ACT_CMD_RESET_CPT_COLOR         40
#define MSG_NON_PRIS_EN_CHARGE          41
#define LIDAR_RET_DEBUG_DEBUG           42 
#define LIDAR_RET_DEBUG_START           43 
#define LIDAR_RET_DEBUG_STOP            44
#define ACT_CMD_SET_MOT4QPos            45
#define PHARE_STATE                     46
#define CMD_PING_UART                   47
#define MSG_NON_PRIS_EN_CHARGE_UART     48
#define LIDAR_RET_DATA_AVAILABLE        100
#define LIDAR_RET_DETECTED_POINTS       101
#define LIDAR_RET_RAW_POINT             102
#define LIDAR_RET_SPEED                 121

#if NB_MOTEUR4Q > 0
  #define TIMER_CN 193
  
  #define MOTEUR_STOP     0
  #define SPEED 		      (-1)
#endif
#if NB_SCREEN > 0
  #include <math.h>
  
  #define BAUDRATE  115200
  
  #define SCREEN_MAIN         0
  #define SREEN_CONTROL       1
  #define SCREEN_TERMINAL     2
  #define SCREEN_PID          3
  #define SCREEN_DEBUG        4

  #define JAUNE               1
  #define BLEU                2
#endif
#if NB_AX12 > 0
  #define BAUDRATE_AX_12 1000000
  #define MID_POS_AX_12  512
  #define ERREUR         0
  #define OK             1
#endif
#if NB_CAPT_COLOR > 0
	#define CORRECTION_LUM
	#define MAX_RGB         1000

	//vitesse du capteur de couleur
	#define TCS230_LOW_FREQ
	//#define TCS230_MEDIUM_FREQ
	//#define TCS230_HIGH_FREQ
 
	#ifdef TCS230_LOW_FREQ
  		#define MAP_RED_MIN   20
  		#define MAP_RED_MAX   3000
  		#define MAP_BLUE_MIN  20
  		#define MAP_BLUE_MAX  3000
  		#define MAP_GREEN_MIN 20
  		#define MAP_GREEN_MAX 3000
  		#define MAP_WHITE_MIN 20
  		#define MAP_WHITE_MAX 3000
	#endif

	#ifdef TCS230_MEDIUM_FREQ
  		#define MAP_RED_MIN   20
  		#define MAP_RED_MAX   300
  		#define MAP_BLUE_MIN  20
  		#define MAP_BLUE_MAX  200
  		#define MAP_GREEN_MIN 20
  		#define MAP_GREEN_MAX 300
  		#define MAP_WHITE_MIN 20
  		#define MAP_WHITE_MAX 300
	#endif

	#ifdef TCS230_HIGH_FREQ
  		#define MAP_RED_MIN   4
  		#define MAP_RED_MAX   60
  		#define MAP_BLUE_MIN  4
  		#define MAP_BLUE_MAX  40
  		#define MAP_GREEN_MIN 4
  		#define MAP_GREEN_MAX 60
  		#define MAP_WHITE_MIN 4
  		#define MAP_WHITE_MAX 60
	#endif
	
	#define AUCUNE	0
	#define ROUGE	 1
	#define VERT	2
	#define BLEU 	3
	#define BLANC 	4
	#define ERREUR	5

	#define MAX_WAIT_COLOR  	  3
	#define NB_ITERATION_COLOR	10

  #define LIM_RED_MIN   1
  #define LIM_RED_MAX   2
  #define LIM_GREEN_MIN 3
  #define LIM_GREEN_MAX 4
  #define LIM_BLUE_MIN  5
  #define LIM_BLUE_MAX  6
  #define LIM_WHITE_MAX 7
#endif
#if NB_UART > 0
  #define BAUDRATE  9600
  
  #define PHARE_OFF 0
  #define PHARE_ON  1
#endif

#endif //SPI_CRIS_LIB_H
