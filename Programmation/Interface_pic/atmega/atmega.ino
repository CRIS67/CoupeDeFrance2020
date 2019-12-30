#include <math.h>

#define LED                 5
#define ADC0                A0
#define SS                  10
#define MOSI                11
#define MISO                12
#define SCK                 13

#define SCREEN_MAIN         0
#define SREEN_CONTROL       1
#define SCREEN_TERMINAL     2
#define SCREEN_PID          3
#define SCREEN_DEBUG        4

#define AUCUN               0
#define POS_X               1
#define POS_Y               2
#define TERM1               3
#define TERM2               4
#define TERM3               5
#define TERM4               6
#define PING                7
#define SCORE               8
#define RESET               9
#define SENS                10
#define SHUTDOWN            11
#define JAUNE               12
#define BLEU                13

#define SPI_IDLE            0
#define SPI_NUM_VAR         1
#define SPI_MSG_UTILE       2
#define SPI_CHECKSUM        3

#define TAILLE_SPI_CHAINE   196
#define TAILLE_SEND         10

int Cpt = 0, EtatScreen = SCREEN_MAIN, AutoLight = 0, FlagSleep = 0, FlagSpi = 1, blinkLed = 0, PosX = 150, PosY = 340, Score = 0, TypeVarSpi = 0, CptPile = 0, PrgmSens = 0, Stop = 0,
TailleMsgSpi = 0, CptSpi = 0, SendNbSpi = 0, CptReadPile = 0, EtatSpi = SPI_IDLE, Checksum = 0, CptSpiSend = 0;
String txt;
char temp, TextSpi[TAILLE_SPI_CHAINE], TabPileSend[TAILLE_SEND], Text[TAILLE_SPI_CHAINE-2][4];

void Affiche(String txt_in);
void(* resetFunc) (void) = 0;

void setup() {
  //led
  digitalWrite(LED, OUTPUT);
  digitalWrite(LED, 0);
  //uart
  Serial.begin(115200);
  //spi
  SPCR = _BV(SPE);    // turn on SPI in slave mode
  SPCR |= _BV(SPIE);  // turn on interrupts
  SPDR = 0;           //clear buffer SPI
}

void loop() {
  delay(80);
  Cpt++;
  /*if(FlagSleep || Stop) {
    digitalWrite(LED,0);
  } else {
    if(Cpt > 10) {
      blinkLed = ~blinkLed;
      if(blinkLed) {
        digitalWrite(LED,1);
      } else {
        digitalWrite(LED,0);
      }
      Cpt = 0;
    }
  }*/
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
}

void Affiche(String txt_in) {
  Serial.print(txt_in);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}

/**************************** ISR ***********************************/
ISR(SPI_STC_vect) {
  char data_spi = SPDR;
  SPDR = 0x03;
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
      if((Checksum)%256 == data_spi) {
        FlagSpi = 1;
        CptSpi = 0;
        switch(TypeVarSpi) {
          case POS_X:
            PosX = TextSpi[0];
            PosX = PosX << 8;
            PosX += TextSpi[1];
            if(PosX > 750) {
              PosX = 750;
            }
            break;
          case POS_Y:
            PosY = TextSpi[0];
            PosY = PosY << 8;
            PosY += TextSpi[1];
            if(PosY > 430) {
              PosY = 430;
            }
            break;
          case PING:
            //a faire
            break;
          case TERM1:
            for(int j=0;j<TAILLE_SPI_CHAINE-2;j++) {
              Text[j][0] = TextSpi[j+2];
            }
            break;
          case TERM2:
            for(int j=0;j<TAILLE_SPI_CHAINE-2;j++) {
              Text[j][1] = TextSpi[j+2];
            }
            break;
          case TERM3:
            for(int j=0;j<TAILLE_SPI_CHAINE-2;j++) {
              Text[j][2] = TextSpi[j+2];
            }
            break;
          case TERM4:
            for(int j=0;j<TAILLE_SPI_CHAINE-2;j++) {
              Text[j][3] = TextSpi[j+2];
            }
            break;
          case SCORE:
            Score = TextSpi[0];
            Score = Score << 8;
            Score += TextSpi[1];
            if(Score > 999) {
              Score = 999;
            }
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
    digitalWrite(LED,1);
    SPDR = 0x03;
    /*CptSpiSend++;
    switch(CptSpiSend) {
      case 1:
        SPDR = 0x03;
        break;
      case 2:
        SPDR = TabPileSend[CptReadPile];
        break;
      case 3:
        switch(TabPileSend[CptReadPile]) {
          case SENS:
            SendNbSpi = PrgmSens;
            PrgmSens = 0;
            break;
          case SHUTDOWN:
            SendNbSpi = Stop;
            Stop = 0;
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
    }*/
  } else {
    SPDR = 0;
  }
}

void serialEvent() {
    char Receive;     //reception de l'ecran
    Receive = Serial.read();
        if(FlagSleep) {
            if(Receive == 0x68) {
                FlagSleep = 0; //a tester
                FlagSpi = 1;
            }
        } else {
            switch(Receive) {
                case 0x31:
                case 0x32:
                case 0x33:
                case 0x34:
                    EtatScreen = Receive-0x30;
                    FlagSpi = 1;
                    break;
                case 0x41:
                case 0x42:
                case 0x43:
                case 0x44:
                    EtatScreen = SCREEN_MAIN;
                    FlagSpi = 1;
                    break;
                case 0x35:
                    AutoLight = 1;
                    break;
                case 0x36:
                    AutoLight = 0;
                    break;
                case 0x40:
                    FlagSleep = 1;
                    break;
                case 0x51:
                    PrgmSens = JAUNE;
                    TabPileSend[CptPile] = SENS;
                    CptPile++;
                    CptPile %= TAILLE_SEND;
                    break;
                case 0x52:
                    PrgmSens = BLEU;
                    TabPileSend[CptPile] = SENS;
                    CptPile++;
                    CptPile %= TAILLE_SEND;
                    break;
                case 0x53:
                    Stop = 1;
                    TabPileSend[CptPile] = SHUTDOWN;
                    CptPile++;
                    CptPile %= TAILLE_SEND;
                    break;
                default:
                    //do nothing
                    break;
            }
  }
}


