#include <Servo.h>
#include <SoftwareSerial.h>

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

#if NB_SERVO > 0
  int Pin_Servo[] = {};
  Servo S[NB_SERVO];
#endif
#if NB_MOTEUR > 0
  int Pin_Moteur[] = {};
#endif
#if NB_MOTEUR4Q > 0
  int Pin_Moteur4Q_SENS[] = {};
  int Pin_Moteur4Q_PWM[] = {};
#endif
#if NB_CAPT_CUR > 0
  int Pin_Capt_Cur[] = {};
#endif
#if NB_CAPT_COLOR > 0

#endif
#if NB_UART > 0
  #define BAUDRATE  9600
#endif
#if NB_RUPT > 0
  int Pin_Rupt[] = {};
#endif
#if NB_AX12 > 0
  int Pin_ax12_RX[NB_AX12];
  int Pin_ax12_TX[NB_AX12];
#endif
#if NB_LIDAR > 0

#endif
#if NB_SCREEN > 0

#endif
#if NB_CAPT_DIST > 0

#endif

//ne pas toucher
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

#if NB_CAPT_CUR > 0
  int Valeur_Cur[NB_CAPT_CUR];
#endif
#if NB_CAPT_COLOR > 0
  int Valeur_Color[NB_CAPT_COLOR];
#endif
#if NB_RUPT > 0
  int Valeur_Rupt[NB_RUPT];
#endif
#if NB_AX12 > 0
  //SoftwareSerial 
  int Ax12[NB_UART];
#endif
#if NB_CAPT_DIST > 0
  int Valeur_Dist[NB_CAPT_DIST];
#endif
#if NB_SCREEN > 0
  int PrgmSens = 0;
  int StopMain = 0;
  int Score = 0;
  int PosX = 200;
  int PosY = 400;
  char Text[5][195];
#endif

#define MOTEUR_STOP     0

void(*resetFunc)(void) = 0; //declare reset function @ address 0

int Cpt = 0, FlagSleep = 0, FlagSpi = 1, TypeVarSpi = 0, CptPile = 0, TailleMsgSpi = 0, CptSpi = 0, SendNbSpi = 0,
CptReadPile = 0, EtatSpi = SPI_IDLE, Checksum = 0, CptSpiSend = 0;
unsigned char TextSpi[TAILLE_SPI_CHAINE], TabPileSend[TAILLE_SEND], TabTypeSend[TAILLE_SEND];

void setup() {
  int i_init;
  #if NB_SERVO > 0
    for(i_init=0;i_init<NB_SERVO;i_init++) {
      S[i_init].attach(Pin_Servo[i_init], SERVO_MIN, SERVO_MAX);
      S[i_init].writeMicroseconds(SERVO_MIN);
    }
  #endif
  #if NB_MOTEUR > 0
    for(i_init=0;i_init<NB_MOTEUR;i_init++) {
      pinMode(Pin_Moteur[i_init], OUTPUT);
      digitalWrite(Pin_Moteur[i_init], LOW);
    }
  #endif
  #if NB_MOTEUR4Q > 0
    for(i_init=0;i_init<NB_MOTEUR4Q;i_init++) {
      pinMode(Pin_Moteur4Q_SENS[i_init], OUTPUT);
      digitalWrite(Pin_Moteur4Q_SENS[i_init], LOW);
      pinMode(Pin_Moteur4Q_PWM[i_init], OUTPUT);
      analogWrite(Pin_Moteur4Q_PWM[i_init], MOTEUR_STOP);
    }
  #endif
  #if NB_RUPT > 0
    for(i_init=0;i_init<NB_MOTEUR4Q;i_init++) {
      pinMode(Pin_Rupt[i_init], INPUT);
    }
  #endif
  #if NB_AX12 > 0
    for(i_init=0;i_init<NB_MOTEUR4Q;i_init++) {
      Ax12[i_init] = SoftwareSerial(Pin_ax12_RX[i_init], Pin_ax12_TX[i_init]);
    }
  #endif
  #if NB_UART > 0
    Serial.begin(BAUDRATE);
  #endif
  

  //spi
  for(i_init=0;i_init<TAILLE_SEND;i_init++) {
    TabPileSend[i_init] = AUCUN;
  }

  SPCR = _BV(SPE);    // turn on SPI in slave mode
  SPCR |= _BV(SPIE);  // turn on interrupts
  SPDR = 0;           //clear buffer SPI

}

void loop() {
  int i_init;
  #if NB_CAPT_CUR > 0
    for(i_init=0;i_init<NB_CAPT_CUR;i_init++) {
      Valeur_Cur[i_init] = analogRead(Pin_Capt_Cur[i_init]);
    }
  #endif
  #if NB_RUPT > 0
    for(i_init=0;i_init<NB_MOTEUR4Q;i_init++) {
      Valeur_Rupt[i_init] = digitalRead(Pin_Rupt[i_init]);
    }
  #endif
}

/**************************** ISR ***********************************/
ISR(SPI_STC_vect) {
  unsigned char data_spi = SPDR;
  switch(EtatSpi) {
    case SPI_IDLE:
      TailleMsgSpi = data_spi;
        if(TailleMsgSpi) {
          EtatSpi = SPI_NUM_VAR;
        }
      Checksum = 0;
      CptSpi = 1;
      break;
    case SPI_NUM_VAR:
      TypeVarSpi = data_spi;
      if(TailleMsgSpi == 2) {
        EtatSpi = SPI_CHECKSUM;
      } else {
        EtatSpi = SPI_MSG_UTILE;
      }
      CptSpi++;
      break;
    case SPI_MSG_UTILE:
      TextSpi[CptSpi-2] = data_spi;
      CptSpi++;
      if(CptSpi == TailleMsgSpi) {
        EtatSpi = SPI_CHECKSUM;
      }
      break;
    case SPI_CHECKSUM:
      if((unsigned char)(Checksum%256) == (unsigned char)(data_spi)) {
        int i_int = 0;
        FlagSpi = 1;
        CptSpi = 0;
        switch(TypeVarSpi) {
          #if NB_SERVO > 0
            case ACT_CMD_SERVO:
                S[TextSpi[0]].writeMicroseconds(TextSpi[1]*256+TextSpi[2]);
              break;
          #endif
          #if NB_MOTEUR > 0
            case ACT_CMD_SET_MOT:
              digitalWrite(Pin_Moteur[TextSpi[0]], TextSpi[1]);
              break;
          #endif
          #if NB_MOTEUR4Q > 0
            case ACT_CMD_SET_MOT4Q:
              digitalWrite(Pin_Moteur4Q_SENS[TextSpi[0]], TextSpi[1]);
              analogWrite(Pin_Moteur4Q_PWM[TextSpi[0]],TextSpi[2]);
              break;
          #endif
          #if NB_CAPT_CUR > 0
            case ACT_CMD_CUR:
              TabPileSend[CptPile] = ACT_CMD_CUR;
              TabTypeSend[CptPile] = TextSpi[0];
              CptPile++;
              CptPile %= TAILLE_SEND;
              break;
          #endif
          #if NB_CAPT_COLOR > 0
            case ACT_CMD_COLOR:
              TabPileSend[CptPile] = ACT_CMD_COLOR;
              TabTypeSend[CptPile] = TextSpi[0];
              CptPile++;
              CptPile %= TAILLE_SEND;
              break;
          #endif
          #if NB_UART > 0
            case ACT_CMD_UART_SEND:
              while(TextSpi[i_int] != '\0') {
                Serial.write(TextSpi[i_int]);
              }
              break;
          #endif
          #if NB_RUPT > 0
            case ACT_CMD_RUPT:
              TabPileSend[CptPile] = ACT_CMD_RUPT;
              TabTypeSend[CptPile] = TextSpi[0];
              CptPile++;
              CptPile %= TAILLE_SEND;
              break;
          #endif
          #if NB_AX12 > 0
            case ACT_CMD_AX12:
              //Serial.write('a');
              //à faire
              break;
          #endif
          #if NB_LIDAR > 0
            //à faire peut être
          #endif
          #if NB_SCREEN > 0
            case HMI_CMD_POS:
              PosX = TextSpi[0];
              PosX = PosX << 8;
              PosX += TextSpi[1];
              if(PosX > 750) {
                PosX = 750;
              }
              PosY = TextSpi[2];
              PosY = PosY << 8;
              PosY += TextSpi[3];
              if(PosY > 430) {
                PosY = 430;
              }
              break;
            case HMI_CMD_TXT:
              for(i_int=0;i_int<TAILLE_SPI_CHAINE-3;i_int++) {
                Text[i_int][TextSpi[2]] = TextSpi[i_int+3];
              }
              break;
            case HMI_CMD_SCORE:
              Score = TextSpi[0];
              Score *= 256;
              Score += TextSpi[1];
              if(Score > 999) {
                Score = 999;
              }
              break;
          #endif
          #if NB_CAPT_DIST > 0
            case ACT_CMD_DIST:
              TabPileSend[CptPile] = ACT_CMD_DIST;
              TabTypeSend[CptPile] = TextSpi[0];
              CptPile++;
              CptPile %= TAILLE_SEND;
              break;
          #endif
          case CMD_PING:
              TabPileSend[CptPile] = CMD_PING;
              CptPile++;
              CptPile %= TAILLE_SEND;
            break;
          case CMD_RST:
            resetFunc();
          break;
          default:
            //do nothing
            break;
        }
      }
      EtatSpi = SPI_IDLE;
      break;
    default:
    EtatSpi = SPI_IDLE;
    break;
  }
  Checksum += data_spi;
  if(TabPileSend[CptReadPile]) {
    CptSpiSend++;
    switch(CptSpiSend) {
      case 1:
        SPDR = 0x04;
        break;
      case 2:
        SPDR = TabPileSend[CptReadPile];
        break;
      case 3:
        SPDR = TabTypeSend[CptReadPile];
        break;
      case 4:
        switch(TabPileSend[CptReadPile]) {
          #if NB_CAPT_CUR > 0
            case ACT_CMD_CUR:
              SendNbSpi = Valeur_Cur[TabTypeSend[CptReadPile]];
              break;
          #endif
          #if NB_CAPT_COLOR > 0
            case ACT_CMD_COLOR:
              SendNbSpi = Valeur_Color[TabTypeSend[CptReadPile]];
              break;
          #endif
          #if NB_RUPT > 0
            case ACT_CMD_RUPT:
              SendNbSpi = Valeur_Rupt[TabTypeSend[CptReadPile]];
              break;
          #endif
          #if NB_CAPT_DIST > 0
            case ACT_CMD_DIST:
              SendNbSpi = Valeur_Dist[TabTypeSend[CptReadPile]];
              break;
          #endif
          case CMD_PING:
            SendNbSpi = 0x01;
            break;
          #if NB_SCREEN > 0
            case HMI_RET_COTE:
              SendNbSpi = PrgmSens;
              PrgmSens = 0;
              break;
            case HMI_RET_OFF_PI:
              SendNbSpi = StopMain;
              StopMain = 0;
              break;
          #endif
          default:
            SendNbSpi = 0;
            break;
        }
        SPDR = SendNbSpi;
        break;
      case 5:
        SPDR = (SendNbSpi+TabPileSend[CptReadPile]+TabTypeSend[CptReadPile]+4)%256;
        TabPileSend[CptReadPile] = AUCUN;
        CptReadPile++;
        CptReadPile %= TAILLE_SEND;
        CptSpiSend = 0;
        SendNbSpi = 0;
        break;
    }
  } else {
    SPDR = 0;
  }
}
