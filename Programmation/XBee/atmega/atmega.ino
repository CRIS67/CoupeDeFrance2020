#include <Servo.h>

#define SS                  10
#define MOSI                11
#define MISO                12
#define SCK                 13
#define SENS_MOT_0          3
#define SENS_MOT_1          4
#define PWM_MOT_0           6
#define PWM_MOT_1           5
#define P_SERVO_0           7
#define P_SERVO_1           A0
#define P_SERVO_2           8
#define P_SERVO_3           9
#define P_RUPT_1            A1
#define P_RUPT_2            A2
#define P_RUPT_3            A1

#define AUCUN               0
#define MOT_SPEED_0         1
#define MOT_SPEED_1         2
#define MOT_SENS_0          3
#define MOT_SENS_1          4
#define SERVO_0             5
#define SERVO_1             6
#define SERVO_2             7
#define SERVO_3             8
#define RUPT_0              9
#define RUPT_1              10
#define RUPT_2              11
#define XBEE                12
#define PING                13
#define RESET               14

#define SPI_IDLE            0
#define SPI_NUM_VAR         1
#define SPI_MSG_UTILE       2
#define SPI_CHECKSUM        3

#define POS_REPO            90

#define TAILLE_SPI_CHAINE   196
#define TAILLE_SEND         10

Servo S0;
Servo S1;
Servo S2;
Servo S3;

int TypeVarSpi = 0, CptPile = 0, TailleMsgSpi = 0, CptSpi = 0, SendNbSpi = 0, CptReadPile = 0, EtatSpi = SPI_IDLE, Checksum = 0,
CptSpiSend = 0, ReceiveXbee;
unsigned char TextSpi[TAILLE_SPI_CHAINE], TabPileSend[TAILLE_SEND];

void(* resetFunc) (void) = 0;

void setup() {
  // I/O
  pinMode(SENS_MOT_0, OUTPUT);
  pinMode(SENS_MOT_1, OUTPUT);
  pinMode(PWM_MOT_0, OUTPUT);
  pinMode(PWM_MOT_1, OUTPUT);
  pinMode(P_RUPT_1, INPUT);
  pinMode(P_RUPT_2, INPUT);
  pinMode(P_RUPT_3, INPUT);
  digitalWrite(SENS_MOT_0,0);
  digitalWrite(SENS_MOT_1,0);
  analogWrite(PWM_MOT_0,0);
  analogWrite(PWM_MOT_1,0);
  // servo
  S0.attach(P_SERVO_0);
  S1.attach(P_SERVO_1);
  S2.attach(P_SERVO_2);
  S3.attach(P_SERVO_3);
  S0.writeMicroseconds(POS_REPO);
  S1.writeMicroseconds(POS_REPO);
  S2.writeMicroseconds(POS_REPO);
  S3.writeMicroseconds(POS_REPO);
  // uart
  Serial.begin(9600);
  // spi
  SPCR = _BV(SPE);    // turn on SPI in slave mode
  SPCR |= _BV(SPIE);  // turn on interrupts
  SPDR = 0;           //clear buffer SPI
}

void loop() {
  delay(80);
  
  
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
        CptSpi = 0;
        switch(TypeVarSpi) {
          case MOT_SPEED_0:
            analogWrite(PWM_MOT_0,TextSpi[0]);
            break;
          case MOT_SPEED_1:
            analogWrite(PWM_MOT_1,TextSpi[0]);
            break;
          case MOT_SENS_0:
            digitalWrite(SENS_MOT_0,TextSpi[0]);
            break;
          case MOT_SENS_1:
            digitalWrite(SENS_MOT_1,TextSpi[0]);
            break;
          case SERVO_0:
            S0.writeMicroseconds(TextSpi[0]*256+TextSpi[1]);
            break;
          case SERVO_1:
            S1.writeMicroseconds(TextSpi[0]*256+TextSpi[1]);
            break;
          case SERVO_2:
            S2.writeMicroseconds(TextSpi[0]*256+TextSpi[1]);
            break;
          case SERVO_3:
            S3.writeMicroseconds(TextSpi[0]*256+TextSpi[1]);
            break;
          case RUPT_0:
              TabPileSend[CptPile] = RUPT_0;
              CptPile++;
              CptPile %= TAILLE_SEND;
            break;
          case RUPT_1:
              TabPileSend[CptPile] = RUPT_1;
              CptPile++;
              CptPile %= TAILLE_SEND;
            break;
          case RUPT_2:
              TabPileSend[CptPile] = RUPT_2;
              CptPile++;
              CptPile %= TAILLE_SEND;
            break;
          case XBEE:
            for(int i=0;i<TextSpi[0];i++) {
              Serial.write(TextSpi[i]);
            }
            break;
          case PING:
            TabPileSend[CptPile] = PING;
            CptPile++;
            CptPile %= TAILLE_SEND;
            break;
          case RESET:
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
        SPDR = 0x03;
        break;
      case 2:
        SPDR = TabPileSend[CptReadPile];
        break;
      case 3:
        switch(TabPileSend[CptReadPile]) {
          case RUPT_0:
            SendNbSpi = digitalRead(P_RUPT_1);
            break;
          case RUPT_1:
            SendNbSpi = digitalRead(P_RUPT_2);
            break;
          case RUPT_2:
            SendNbSpi = digitalRead(P_RUPT_3);
            break;
          case PING:
            SendNbSpi = 1;
            break;
          case XBEE:
            SendNbSpi = ReceiveXbee;
            break;
        }
        SPDR = SendNbSpi;
        break;
      case 4:
        SPDR = (SendNbSpi+TabPileSend[CptReadPile]+3)%256;
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

void serialEvent() {
    ReceiveXbee = Serial.read();
    TabPileSend[CptPile] = XBEE;
    CptPile++;
    CptPile %= TAILLE_SEND;
}


