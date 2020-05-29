#include "SPI_CRIS_lib.hpp"

int Cpt = 0, TypeVarSpi = 0, CptPile = 0, TailleMsgSpi = 0, CptSpi = 0, SendNbSpi = 0, CptReadPile = 0, EtatSpi = SPI_IDLE,
CptSpiSend = SPI_SEND_IDLE, Checksum = 0,CS_fault = 0, SendNbSpi2 = 0;
unsigned char TextSpi[TAILLE_SPI_CHAINE], TabPileSend[TAILLE_SEND], TabTypeSend[TAILLE_SEND], TabTailleSend[TAILLE_SEND];
volatile unsigned long timeLed, timeCS;

void(*resetFunc)(void) = 0; //declare reset function @ address 0

/*****************COM_SPI*****************/
#ifdef COM_SPI
  ISR(SPI_STC_vect) {
    SPDR = ISRCrisSpi(SPDR);
  }
  void SetupCOM_SPI(void) {
  	for(int i_init=0;i_init<TAILLE_SEND;i_init++) {
      TabPileSend[i_init] = AUCUN;
    }
    pinMode(SS, INPUT);
    pinMode(MOSI, INPUT);
    pinMode(MISO, OUTPUT);
    pinMode(SCK, INPUT);
    SPCR = _BV(SPE);    // turn on SPI in slave mode
    SPCR |= _BV(SPIE);  // turn on interrupts
    SPDR = 0;           //clear buffer SPI
  }
#endif
/*****************COM_UART*****************/
#ifdef COM_UART
  uint8_t PhareState = PHARE_OFF;
  uint8_t BlinkPhare = 0;
  void LoopCOM_UART(void) {
  	BlinkPhare = ~BlinkPhare;
  }
#endif
/*****************SERVO*****************/
#if NB_SERVO > 0
  Servo S[NB_SERVO];
  void SetupServo(void) {
	for(int i_init=0;i_init<NB_SERVO;i_init++) {
      #ifdef SERVO_MIN_MAX
        S[i_init].attach(Pin_Servo[i_init], SERVO_MIN, SERVO_MAX);
      #else
        S[i_init].attach(Pin_Servo[i_init]);
      #endif
      S[i_init].writeMicroseconds(SERVO_MIN);
    }
  }
#endif
  /*****************MOTEUR4Q*****************/
#if NB_MOTEUR4Q > 0
  int PosGo[NB_MOTEUR4Q], PosActu[NB_MOTEUR4Q];
  void SetupMoteur4Q(void) {
  	TCCR2A = 0;
    TCCR2B = 0b00000110; // 1/256
    TIMSK2 = 0b00000001;
    TCNT2 = TIMER_CN;
    sei();
    for(int i_init=0;i_init<NB_MOTEUR4Q;i_init++) {
      pinMode(Pin_Moteur4Q_SENS[i_init], OUTPUT);
      digitalWrite(Pin_Moteur4Q_SENS[i_init], LOW);
      pinMode(Pin_Moteur4Q_PWM[i_init], OUTPUT);
      analogWrite(Pin_Moteur4Q_PWM[i_init], MOTEUR_STOP);
      PosGo[i_init] = SPEED;
    }
  }
  void loopMoteur4Q(void) {
  	for(int i_init = 0;i_init<NB_MOTEUR4Q;i_init++) {
      if(digitalRead(Pin_RuptEnd4Q[i_init][1])) {
        analogWrite(Pin_Moteur4Q_PWM[i_init],MOTEUR_STOP);
        PosGo[i_init] = SPEED;
      }
      if(digitalRead(Pin_RuptEnd4Q[i_init][0])) {
        analogWrite(Pin_Moteur4Q_PWM[i_init],MOTEUR_STOP);
        PosGo[i_init] = SPEED;
        PosActu[i_init] = 0;
      }
    }
  }
  ISR (TIMER2_OVF_vect) {
    TCNT2 = TIMER_CN;
    for(int i_intT=0;i_intT<NB_MOTEUR4Q;i_intT++) {
  	  if(PosGo[i_intT] > 0) {
        PosGo[i_intT]--;
      } else if(PosGo[i_intT] == 0) {
        analogWrite(Pin_Moteur4Q_PWM[i_intT],MOTEUR_STOP);
        PosGo[i_intT] = SPEED;
      }
    }
  }
#endif
/*****************MOTEUR*****************/
#if NB_MOTEUR > 0
  void SetupMoteur(void) {
  	for(int i_init=0;i_init<NB_MOTEUR;i_init++) {
      pinMode(Pin_Moteur[i_init], OUTPUT);
      digitalWrite(Pin_Moteur[i_init], LOW);
    }
  }
#endif
/*****************CAPT_CUR*****************/
#if NB_CAPT_CUR > 0
  int Valeur_Cur[NB_CAPT_CUR];
  void LoopCaptCur(void) {
  	for(int i_init=0;i_init<NB_CAPT_CUR;i_init++) {
      Valeur_Cur[i_init] = analogRead(Pin_Capt_Cur[i_init]);
    }
  }
#endif
/*****************CAPT_COLOR*****************/
#if NB_CAPT_COLOR > 0
  //limite des couleurs 40-135(6)-420(1)-3000
  int TabSeuil[] = {15, 39, 39, 100, 195, 350, 32};
  int cpt_capt_color[NB_CAPT_COLOR], Hg[NB_CAPT_COLOR], Valeur_Color[NB_CAPT_COLOR]; 
  char GetColor(unsigned char nb_bras) {
    int period, red, green, blue;
    double H, C, m, M, white;
    unsigned char col;
    if(cpt_capt_color[nb_bras] < MAX_WAIT_COLOR && nb_bras > -1 && nb_bras < NB_CAPT_COLOR) { 
      // temps d'exec : 400-800us
      // Lecture de la composante rouge
      digitalWrite(Pin_Capt_Color_TS2,LOW);
      digitalWrite(Pin_Capt_Color_TS3,LOW);
      delayMicroseconds(10);
      //period = pulseIn(Pin_Capt_Color_Arm[nb_bras], LOW);
      period = pulseIn(A0, LOW);
      if(MAP_RED_MIN >= period) {
        cpt_capt_color[nb_bras]++;
      } else {
        cpt_capt_color[nb_bras] = 0;
        red = map(period, MAP_RED_MIN, MAP_RED_MAX,MAX_RGB,0);
        // Lecture de la composante verte
        digitalWrite(Pin_Capt_Color_TS2,HIGH);
        digitalWrite(Pin_Capt_Color_TS3,HIGH);
        delayMicroseconds(10);
        period = pulseIn(Pin_Capt_Color_Arm[nb_bras], LOW);
        green = map(period, MAP_GREEN_MIN,MAP_GREEN_MAX,MAX_RGB,0);
        // Lecture de la composante bleue
        digitalWrite(Pin_Capt_Color_TS2,LOW);
        digitalWrite(Pin_Capt_Color_TS3,HIGH);
        delayMicroseconds(10);
        period = pulseIn(Pin_Capt_Color_Arm[nb_bras], LOW);
        blue = map(period, MAP_BLUE_MIN,MAP_BLUE_MAX,MAX_RGB,0);
        // Lecture de la composante blanche
        digitalWrite(Pin_Capt_Color_TS2,HIGH);
        digitalWrite(Pin_Capt_Color_TS3,LOW);
        delayMicroseconds(10);
        period = pulseIn(Pin_Capt_Color_Arm[nb_bras], LOW);
        white = period;
        //adaptation de la luminosité
        white = -0.2*white+45; //test avec 40 ou 30 (45)
        // Conversion de la couleur RGB en couleur HSL
        //prérequis
        M = max(max(red, green), blue);
        m = min(min(red, green), blue);
        C = M - m;
        //HSL
        if(C!=0){
          if(M == red) {
            H = green-blue;
            H /= C;
          } else if(M == green) {
            H = blue-red;
            H/= C;
            H += 2;
          } else {
            H = red-green;
            H /= C;
            H += 4;
          }
        } else {
          H = -1;
        }
        H *= 60;
        if(H < 0) {H *= -1;}
        #ifdef CORRECTION_LUM
          H += white;
        #endif
        // Déduction de la couleur à partir du HSL
        Hg[nb_bras] = H;
        if(white > LIM_WHITE_MAX) {
          col = 4;
        } else if(LIM_RED_MIN < H && H < LIM_RED_MAX) {
          col = 1;
        } else if(LIM_GREEN_MIN < H && H < LIM_GREEN_MAX) {
          col = 2;
        } else if(LIM_BLUE_MIN < H && H < LIM_BLUE_MAX) {
          col = 3;
        } else {
          col = 0;
        }
        return col;
      }
    } else {
      return 5;
    }
  }
  unsigned char RatioErr(unsigned char nb_bras, int taille) {
    int i, R = 0, G = 0, B = 0, E = 0, C = 0, W = 0, test;
    for(i=0;i<taille;i++) {
      test = GetColor(nb_bras);
      switch(test) {
        case 0:
          E++;
          break;
        case 1:
          R++;
          break;
        case 2:
          G++;
          break;
        case 3:
          B++;
          break;
        case 4:
          W++;
          break;
        case 5:
          C++;
          break;  
      }
    }
    if(E >= int(taille/2)) {
      return 0;
    } else if(W >= int(taille/2)) {
      return 4;
    } else if(C >= int(taille/2)) {
      return 5;
    } else if(max(E,max(R,max(B,G))) == R) {
      return 1;
    } else if(max(E,max(B,G)) == G) {
      return 2;
    } else if(max(E,B) == B) {
      return 3;
    } else {
      return 0;
    }
  }
  void SetupCaptColor(void) {
  	#ifdef PIN_ENABLE
      pinMode(Pin_Capt_Color_Ena,OUTPUT);
      digitalWrite(Pin_Capt_Color_Ena, LOW);
    #endif
    pinMode(Pin_Capt_Color_TS2,OUTPUT);
    pinMode(Pin_Capt_Color_TS3,OUTPUT);
    digitalWrite(Pin_Capt_Color_TS2,LOW);
    digitalWrite(Pin_Capt_Color_TS3,LOW);
    for(int i_init=0;i_init<NB_CAPT_COLOR;i_init++) {
      pinMode(Pin_Capt_Color_Arm[i_init],INPUT);
      cpt_capt_color[i_init] = 0;
    }
  }
  void LoopCaptColor(void) {
  	for(int i_init=0;i_init<NB_CAPT_COLOR;i_init++) {
      Valeur_Color[i_init] = RatioErr(i_init, NB_ITERATION_COLOR);
    }
  }
#endif
/*****************UART*****************/
#if NB_UART > 0
  char Flag = 'c';                  // Caractère indiquant le début d'un message
  String Data_Text = "";            // Chaine contenant le dernier message reçu
  int Data_Type = 0;                // Type du dernier message reçu
  int Data_Sender = 0;              // Adresse proprede la XBee qui à envoyé ledit message
  boolean Data_Reading = false;
  static void XBee_Config(String Sender, String Receiver, String Network) { // Configure la XBee. Le programme reste bloqué si il ne détecte pas la XBee
    char thisByte = 0;
    while (Serial.available() > 0) {
      if (thisByte != '\r') {
        thisByte = Serial.read();
      }
    }
    Serial.print("ATRE\r");
    Serial.print("ATMY" + Sender + "\r");
    Serial.print("ATDL" + Receiver + "\r");
    Serial.print("ATID" + Network + "\r");
    Serial.print("ATCN\r");
  }
  static void XBee_Send(int adress, int type, String data0) {   // Envoit un message
    int n = data0.length();
    String data;
    data = String(Flag) + char(adress) + char(XBee_Adress) + char(type) + char(n)  + data0;
    int chk = data.length();
    data = data + char(chk);
    Serial.print(data);
  }
  void serialEvent() {
    while(Serial.available()) {
      char c = (char)(Serial.read());
      static String received;
      static int i;           // Emplacement du caractère dans le message
      static int n;           // Longueur du message
      static char adress;     // Adresse de l'envoyeur
      if (Data_Reading) {     // Data_Reading passe à True lorsque le début d'un message est détecté, afin de ne pas lire un message si un autre est en cours de lecture
        received += char(c);  // C contient le dernier caractère reçu
      if (i == 1)           // Certaines positions correspondant à des données utiles de reception sont vérifiées
        adress = char(c);
      if (i == 4)
        n = int(c);
      } else if (c == Flag && !Data_Reading) {
        received = "";
        i = 0;
        n = 0;
        Data_Reading = true;
      }
      i += 1;
      if (i == n + 6) {             // Verification de la longueur de la trame : le dernier caractère est égal à celle-ci.
        Data_Reading = false;
        int l = received.length();  // Longueur totale de la trame
        if (adress == char(XBee_Adress) && l == int(received.charAt(l - 1)) ) // Le message n'est validé que si l'adresse et la longueur sont correctes
          Data_Text = received.substring(4, l - 1);
          Data_Type = int(received.charAt(2));
          Data_Sender = int(received.charAt(1));
        }
      }
  }
  static void Data_Clear() {      // Supprime les données reçues
    Data_Text = "";
    Data_Type = 0;
    Data_Sender = 0;
  }
  void SetupUART(void) {
  	Serial.begin(BAUDRATE);
    //delay(6000);
    #ifdef COM_UART
      String XBee_Team = "1";
      String Other_Team = "2";
    #else
      String XBee_Team = "2";
      String Other_Team = "1";
    #endif
    XBee_Config(XBee_Team, Other_Team, Network_Adress);
    delay(1000);
    XBee_Send(Other_Adress, CMD_PING_UART, "0");
  }
  void LoopUART(void) {
  	#ifdef COM_UART
      if(PhareState == PHARE_ON) {
        if(digitalRead(Pin_Rupt[0])) {
          analogWrite(Pin_Moteur4Q_PWM[1], 0);
          digitalWrite(LED_PHARE, 1);
          if(BlinkPhare) {
            S[0].writeMicroseconds(SERVO_MIN);
          } else {
            S[0].writeMicroseconds(SERVO_MAX);
          }
        } else {
          digitalWrite(Pin_Moteur4Q_SENS[1], 1);
          analogWrite(Pin_Moteur4Q_PWM[1], 255);
        }
      }
      if(Data_Text != "") {
    	  switch(Data_Type) {
    		case CMD_RST:
    			resetFunc();
    			break;
    		case CMD_PING_UART:
    			XBee_Send(Data_Sender, CMD_PING_UART, "0");
    			break;
    		case PHARE_STATE:
    			if(Data_Text.substring(0,1) == "1") {
    				PhareState = PHARE_ON;
            digitalWrite(LED_PHARE, 1);
            digitalWrite(Pin_Moteur4Q_SENS[1], 1);
            analogWrite(Pin_Moteur4Q_PWM[1], 255);
    			} else {
    				PhareState = PHARE_OFF;
            digitalWrite(LED_PHARE, 0);
    			}
    			break;
    		default:
    			XBee_Send(Data_Sender, MSG_NON_PRIS_EN_CHARGE_UART, "0");
    			break;
    	  }
        Data_Clear();
      }
    #else
      if(Data_Text  != "") {
    	  switch(Data_Type) {
    		case CMD_PING_UART:
    			SendSpi(CMD_PING_UART,3,0);
    			break;
    		case MSG_NON_PRIS_EN_CHARGE_UART:
    			SendSpi(MSG_NON_PRIS_EN_CHARGE_UART,3,0);
    			break;
    		default:
    			SendSpi(MSG_NON_PRIS_EN_CHARGE_UART,3,0);
    			break;
    	  }
        Data_Clear();
      }
  	#endif
  }
#endif
/*****************RUPT*****************/
#if NB_RUPT > 0
  int Valeur_Rupt[NB_RUPT];
  void SetupRupt(void) {
  	for(int i_init=0;i_init<NB_RUPT;i_init++) {
      pinMode(Pin_Rupt[i_init], INPUT);
    }
  }
  void LoopRupt(void) {
  	for(int i_init=0;i_init<NB_RUPT;i_init++) {
      Valeur_Rupt[i_init] = digitalRead(Pin_Rupt[i_init]);
    }
  }
#endif
/*****************AX12*****************/
#if NB_AX12 > 0
  DynamixelClass Dynamixel;
  int ax12_pos[NB_AX12];
  int IsAx12Ok[NB_AX12];
  void SetupAx12(void) {
  	for(int i_init=1;i_init<NB_AX12+1;i_init++) {
      Dynamixel.begin(BAUDRATE_AX_12, PinDir);
      Dynamixel.setEndless(i_init, OFF);
      if(Dynamixel.ping(i_init)) {
        Dynamixel.reset(i_init);
        delay(200);
        if(Dynamixel.ping(i_init)) {
          IsAx12Ok[i_init-1] = ERREUR;
        } else {
          Dynamixel.move(i_init,MID_POS_AX_12);
          ax12_pos[i_init-1] = MID_POS_AX_12;
          IsAx12Ok[i_init-1] = OK;
        }
      } else {
        Dynamixel.move(i_init,MID_POS_AX_12);
        ax12_pos[i_init-1] = MID_POS_AX_12;
        IsAx12Ok[i_init-1] = OK;
      }
    }
  }
  void LoopAx12(void) {
  	for(int i_init = 1;i_init < NB_AX12;i_init++) {
      if(IsAx12Ok[i_init-1] == OK) {
        Dynamixel.move(i_init,ax12_pos[i_init-1]);
      }
    }
  }
#endif
/*****************SCREEN*****************/
#if NB_SCREEN > 0
  char Text[TAILLE_SPI_CHAINE-2][4], temp;
  String txt;
  int AutoLight = 0, EtatScreen = SCREEN_MAIN, blinkLed = 0, PrgmSens = 0, StopMain = 0, Score = 0, PosX = 200, PosY = 400, FlagSpi = 1, FlagSleep = 0;
  void Affiche(String txt_in) {
    Serial.print(txt_in);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
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
                    SendSpi(HMI_RET_COTE,3,0);
                    break;
                case 0x52:
                    PrgmSens = BLEU;
                    SendSpi(HMI_RET_COTE,3,0);
                    break;
                case 0x53:
                    StopMain = 1;
                    SendSpi(HMI_RET_OFF_PI,3,0);
                    break;
                default:
                    //do nothing
                    break;
            }
    }
  }
  void SetupScreen(void) {
  	Serial.begin(BAUDRATE);
  }
  void LoopScreen(void) {
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
  }
#endif
/*****************CAPT_DIST*****************/
#if NB_CAPT_DIST > 0
  int Valeur_Dist[NB_CAPT_DIST];
#endif
/*****************SEND_SPI*****************/
void ReceiveSpi(void) {
	String inp;
    int i_int = 0;
    CptSpi = 0;
    switch(TypeVarSpi) {
      #if NB_SERVO > 0
        case ACT_CMD_SERVO:
          S[TextSpi[0]].writeMicroseconds(TextSpi[1]*256+TextSpi[2]);
          SendSpi(CMD_ACK,3,0);
          break;
      #endif
      #if NB_MOTEUR > 0
        case ACT_CMD_SET_MOT:
          digitalWrite(Pin_Moteur[TextSpi[0]], TextSpi[1]);
          SendSpi(CMD_ACK,3,0);
          break;
      #endif
      #if NB_MOTEUR4Q > 0
        case ACT_CMD_SET_MOT4QVit:
          digitalWrite(Pin_Moteur4Q_SENS[TextSpi[0]], TextSpi[1]);
          analogWrite(Pin_Moteur4Q_PWM[TextSpi[0]],TextSpi[2]);
          SendSpi(CMD_ACK,3,0);
          break;
        case ACT_CMD_SET_MOT4QPosNorm:
          PosGo[TextSpi[0]] = TextSpi[1]*256+TextSpi[2]*Length_Moteur[TextSpi[0]]-PosActu[TextSpi[0]];
          PosActu[TextSpi[0]] = TextSpi[1]*256+TextSpi[2]*Length_Moteur[TextSpi[0]];
          if(PosGo[TextSpi[0]] > 0) {
            digitalWrite(Pin_Moteur4Q_SENS[TextSpi[0]], 1);
            analogWrite(Pin_Moteur4Q_PWM[TextSpi[0]],Speed_Moteur4Q[TextSpi[0]][1]);
          } else {
            digitalWrite(Pin_Moteur4Q_SENS[TextSpi[0]], 0);
            analogWrite(Pin_Moteur4Q_PWM[TextSpi[0]],Speed_Moteur4Q[TextSpi[0]][0]);
            PosGo[TextSpi[0]] *= -1;
          }
          SendSpi(CMD_ACK,3,0);
          break;
        case ACT_CMD_SET_MOT4QPos:
          digitalWrite(Pin_Moteur4Q_SENS[TextSpi[0]], TextSpi[1]);
          analogWrite(Pin_Moteur4Q_PWM[TextSpi[0]], TextSpi[2]);
          PosGo[TextSpi[0]] = TextSpi[3];
      #endif
      #if NB_CAPT_CUR > 0
        case ACT_CMD_CUR:
          SendSpi(ACT_CMD_CUR,4,TextSpi[0]);
          break;
      #endif
      #if NB_CAPT_COLOR > 0
        case ACT_CMD_COLOR:
          SendSpi(ACT_CMD_COLOR,4,TextSpi[0]);
          break;
        case ACT_CMD_SEUIL_COLOR:
          TabSeuil[TextSpi[0]] = TextSpi[1]*256+TextSpi[2];
          break;
        case ACT_CMD_RESET_CPT_COLOR:
          for(i_int=0;i_int<NB_CAPT_COLOR;i_int++) {
            cpt_capt_color[i_int] = 0;
          }
          break;
      #endif
      #if NB_UART > 0
      #ifndef COM_UART
        case ACT_CMD_UART_SEND:
          i_int = 2;
          inp = "";
          while(TextSpi[i_int] != '\0') {
            inp += (char)(TextSpi[i_int]);
            i_int++;
          }
          XBee_Send(TextSpi[1], TextSpi[0], inp);
          break;
      #endif
      #endif
      #if NB_RUPT > 0
        case ACT_CMD_RUPT:
          SendSpi(ACT_CMD_RUPT,3,TextSpi[0]);
          break;
      #endif
      #if NB_AX12 > 0
        case ACT_CMD_AX12:
          ax12_pos[TextSpi[0]-1] = TextSpi[1]*256+TextSpi[2];
          SendSpi(CMD_ACK,3,0);
          break;
      #endif
      #if NB_SCREEN > 0
        case HMI_CMD_POS:
        	FlagSpi = 1;
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
    	  FlagSpi = 1;
          for(i_int=0;i_int<TAILLE_SPI_CHAINE-3;i_int++) {
            Text[i_int][TextSpi[2]] = TextSpi[i_int+3];
          }
          break;
        case HMI_CMD_SCORE:
        	FlagSpi = 1;
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
          SendSpi(ACT_CMD_DIST,4,TextSpi[0]);
          break;
      #endif
        case CMD_PING:
          SendSpi(CMD_PING,3,0);
          break;
        case CMD_RST:
          resetFunc();
          break;
        default:
          SendSpi(MSG_NON_PRIS_EN_CHARGE,3,0);
          break;
    }
}
/*****************RECEIVE_SPI*****************/
void SendSpi8bitL(void) {
  switch(TabPileSend[CptReadPile]) {
    #if NB_CAPT_CUR > 0
      case ACT_CMD_CUR:
        SendNbSpi = (uint8_t)(Valeur_Cur[TabTypeSend[CptReadPile]]/256);
        break;
    #endif
    #if NB_CAPT_COLOR > 0
      case ACT_CMD_COLOR:
        //SendNbSpi = (uint8_t)(Valeur_Color[TabTypeSend[CptReadPile]]/256);
        SendNbSpi = (uint8_t)(Hg[TabTypeSend[CptReadPile]]/256);
        break;
    #endif
    #if NB_RUPT > 0
      case ACT_CMD_RUPT:
        SendNbSpi = Valeur_Rupt[TabTypeSend[CptReadPile]];
        break;
    #endif
    #if NB_CAPT_DIST > 0
      case ACT_CMD_DIST:
        SendNbSpi = (uint8_t)(Valeur_Dist[TabTypeSend[CptReadPile]]/256);
        break;
    #endif
    #if NB_SCREEN > 0
      case HMI_RET_COTE:
        SendNbSpi = PrgmSens;
        break;
      case HMI_RET_OFF_PI:
        SendNbSpi = StopMain;
        StopMain = 0;
        break;
    #endif
      case CMD_PING:
        SendNbSpi = 0x01;
        break;
      default:
        SendNbSpi = 0;
        break;
  }
}
void SendSpi8bitH(void) {
  switch(TabPileSend[CptReadPile]) {
    #if NB_CAPT_CUR > 0
      case ACT_CMD_CUR:
        SendNbSpi2 = Valeur_Cur[TabTypeSend[CptReadPile]]%256;
        break;
    #endif
    #if NB_CAPT_COLOR > 0
      case ACT_CMD_COLOR:
        //SendNbSpi2 = Valeur_Color[TabTypeSend[CptReadPile]]%256;
        SendNbSpi2 = Hg[TabTypeSend[CptReadPile]]%256;
        break;
    #endif
    #if NB_CAPT_DIST > 0
      case ACT_CMD_DIST:
        SendNbSpi2 = Valeur_Dist[TabTypeSend[CptReadPile]]%256;
        break;
    #endif
      default:
        SendNbSpi2 = 0;
        break;
  }
}
/*****************SETUP*****************/
void InitCrisSpi(void) {
  pinMode(Pin_Led, OUTPUT);
  digitalWrite(Pin_Led, 1);
  int i_init;
  #if NB_SERVO > 0
    SetupServo();
  #endif
  #if NB_MOTEUR > 0
    SetupMoteur();
  #endif
  #if NB_MOTEUR4Q > 0
    SetupMoteur4Q();
  #endif
  #if NB_RUPT > 0
    SetupRupt();
  #endif
  #if NB_AX12 > 0
    SetupAx12();
  #endif
  #if NB_UART > 0
    SetupUART();
  #endif
  #if NB_SCREEN > 0
    SetupScreen();
  #endif
  #if NB_CAPT_COLOR > 0
    SetupCaptColor();
  #endif
  #ifdef COM_SPI
    SetupCOM_SPI();
  #endif
  timeLed = millis();
}
/*****************LOOP*****************/
void LoopCrisSpi(void) {
  int i_init;
  #if NB_AX12 > 0
    LoopAx12();
  #endif
  #if NB_CAPT_CUR > 0
    LoopCaptCur();
  #endif
  #if NB_MOTEUR4Q > 0
    loopMoteur4Q();
  #endif
  #if NB_RUPT > 0
    LoopRupt();
  #endif
  #if NB_SCREEN > 0
    LoopScreen();
  #else
    if(millis() - timeLed > 1000){ 
      timeLed = millis();
      digitalWrite(Pin_Led,!digitalRead(Pin_Led));
      #ifdef COM_UART
        LoopCOM_UART();
      #endif
    }
  #endif
  #if NB_CAPT_COLOR > 0
    LoopCaptColor();
  #endif
  #if NB_UART > 0
  	LoopUART();
  #endif
  if(CS_fault) {
    if(millis() - timeCS > 100){ 
      CS_fault = 0;
      EtatSpi = SPI_IDLE;
    }
  }
}
/*****************SPI*****************/
unsigned char ISRCrisSpi(unsigned char data_spi) {
  timeCS = millis();
  if(!CS_fault) {
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
          ReceiveSpi();
        } else {
          CS_fault = 1;
          timeCS = millis();
        }
        EtatSpi = SPI_IDLE;
        break;
      default:
        EtatSpi = SPI_IDLE;
        break;
    }
    Checksum += data_spi;
    if(TabPileSend[CptReadPile]) {
      switch(CptSpiSend) {
        case SPI_SEND_IDLE:
          data_spi = TabTailleSend[CptReadPile];
          CptSpiSend = SPI_SEND_TYPE;
          break;
        case SPI_SEND_TYPE:
          data_spi = TabPileSend[CptReadPile];
          CptSpiSend = SPI_SEND_NUM;
          break;
        case SPI_SEND_NUM:
          data_spi = TabTypeSend[CptReadPile];
          CptSpiSend = SPI_SEND_MSG;
          break;
        case SPI_SEND_MSG:
          SendSpi8bitL();
          data_spi = SendNbSpi;
          if(TabTailleSend[CptReadPile] > 5) {
            CptSpiSend = SPI_SEND_MSG2;
          } else {
            CptSpiSend = SPI_SEND_CHECK;
          }
          break;
        case SPI_SEND_MSG2:
          SendSpi8bitH();
          data_spi = SendNbSpi2;
          CptSpiSend = SPI_SEND_CHECK;
          break;
        case SPI_SEND_CHECK:
          data_spi = (SendNbSpi+TabPileSend[CptReadPile]+TabTypeSend[CptReadPile]+TabTailleSend[CptReadPile]+SendNbSpi2)%256;
          TabPileSend[CptReadPile] = AUCUN;
          CptSpiSend = SPI_SEND_IDLE;
          CptReadPile++;
          CptReadPile %= TAILLE_SEND;
          SendNbSpi = 0;
          SendNbSpi2 = 0;
          break;
      }
    } else {
      data_spi = 0;
    }
  }
  return data_spi;
}
void SendSpi(uint8_t buf, uint8_t leng, uint8_t num) {
  TabPileSend[CptPile] = buf;
  TabTailleSend[CptPile] = leng+2;
  TabTypeSend[CptPile] = num;
  CptPile++;
  CptPile %= TAILLE_SEND;
}
