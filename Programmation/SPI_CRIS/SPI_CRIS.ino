#include <Servo.h>
#include <SoftwareSerial.h>

//à modifier
#define NB_SERVO        3
#define NB_MOTEUR       3
#define NB_MOTEUR4Q     2
#define NB_CAPT_CUR     3
#define NB_CAPT_COLOR   3
#define NB_UART         2
#define NB_RUPT         2
#define NB_AX12         2
#define NB_LIDAR        2
#define NB_SCREEN       2
#define NB_CAPT_DIST    2

#define SERVO_MIN     700
#define SERVO_MAX     1600

const int Pin_Led;
#if NB_SERVO > 0
  const int Pin_Servo[] = {};
  Servo S[NB_SERVO];
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
#if NB_UART > 0
  #define BAUDRATE  9600
#endif
#if NB_SCREEN > 0
  #include <math.h>
  #define BAUDRATE  115200
  #define ADC0                A0
  #define SCREEN_MAIN         0
  #define SREEN_CONTROL       1
  #define SCREEN_TERMINAL     2
  #define SCREEN_PID          3
  #define SCREEN_DEBUG        4
  void Affiche(String txt_in);
  char Text[TAILLE_SPI_CHAINE-2][4], temp;
  String txt;
  int AutoLight = 0, EtatScreen = SCREEN_MAIN, blinkLed = 0, PrgmSens = 0, StopMain = 0, Score = 0, PosX = 200, PosY = 400;
  void Affiche(String txt_in) {
    Serial.print(txt_in);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
  }
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
  #if NB_SCREEN > 0
    Serial.begin(BAUDRATE);
    digitalWrite(Pin_Led, OUTPUT);
    digitalWrite(Pin_Led, 0);
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
  #if NB_SCREEN > 0
    delay(80);
    Cpt++;
    if(FlagSleep || StopMain) {
      digitalWrite(Pin_Led,0);
    } else {
      if(Cpt > 10) {
        blinkLed = ~blinkLed;
        if(blinkLed) {
          digitalWrite(Pin_Led,1);
        } else {
          digitalWrite(Pin_Led,0);
        }
        Cpt = 0;
      }
    }
    if(AutoLight) {
      double adc_res = analogRead(ADC0);
      adc_res = 103*exp(-(adc_res/610));
      adc_res = (int)(adc_res);
      if(adc_res < 0) {
        adc_res = 0;
      }
      if(adc_res > 99) {
        adc_res = 99;
      }
      txt = "dim=";
      temp = '0' + (char)(adc_res/10);
      txt += temp;
      temp = '0' + (char)(adc_res-(10*(char)(adc_res/10)));  //regressi
      txt += temp;
      Affiche(txt);
      if(EtatScreen == SCREEN_MAIN) {
        txt = "h0.val=";
        temp = '0' + (char)(adc_res/10);
        txt += temp;
        temp = '0' + (char)(adc_res-(10*(char)(adc_res/10)));
        txt += temp;
        Affiche(txt);
      }
    }
    if(FlagSpi) {
      FlagSpi = 0;
      switch(EtatScreen) {
        case SCREEN_MAIN:
          txt = "n0.val=";
          temp = '0' + (char)(Score/100);
          txt += temp;
          temp = '0' + (char)((Score-((char)(Score/100)*100))/10);
          txt += temp;
          temp = '0' + (char)((Score-((char)(Score/10)*10)));
          txt += temp;
          Affiche(txt);
          break;
        case SREEN_CONTROL:
          FlagSpi = 1;
          Affiche("ref 0");
          txt = "pic ";
          temp = '0' + (char)(PosX/100);
          txt += temp;
          temp = '0' + (char)((PosX-(100*(char)(PosX/100)))/10);
          txt += temp;
          temp = '0' + (char)(PosX-(10*(char)(PosX/10)));
          txt += temp;
          txt += ",";
          temp = '0' + (char)(PosY/100);
          txt += temp;
          temp = '0' + (char)((PosY-(100*(char)(PosY/100)))/10);
          txt += temp;
          temp = '0' + (char)(PosY-(10*(char)(PosY/10)));
          txt += temp;
          txt += ",0";
          //txt = "pic 150,150,0";
          Affiche(txt);
          break;
        case SCREEN_TERMINAL:
          //printS("t0.txt=\"mont_text\"ÿÿÿ");txt = "t";
          txt = "t0.txt=\"";
          for(int k=0;k<TAILLE_SPI_CHAINE-2;k++) {
            txt += Text[k][0];
          }
          txt += "\"";
        
          txt = "t1.txt=\"";
          for(int k=0;k<TAILLE_SPI_CHAINE-2;k++) {
            txt += Text[k][1];
          }
          txt += "\"";

          txt = "t2.txt=\"";
          for(int k=0;k<TAILLE_SPI_CHAINE-2;k++) {
            txt += Text[k][2];
          }
          txt += "\"";

          txt = "t3.txt=\"";
          for(int k=0;k<TAILLE_SPI_CHAINE-2;k++) {
            txt += Text[k][3];
          }
          txt += "\"";
          break;
        case SCREEN_PID:
          //graph
          break;
        case SCREEN_DEBUG:
          //a faire
          break;
        default:
          EtatScreen = SCREEN_MAIN;
          break;
      }
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
