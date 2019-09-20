#include <Servo.h>

//affiche plus ou moins d'informations à commenter/décommenter
//#define DEBUG_NORM
#define DEBUG_ERR
//#define DEBUG_ACT
//#define DEBUG_COL
//#define DEBUG_RATIO
//#define DEBUG_SPI

//define de test
#define DEBUG_NORM_NB   1
#define DEBUG_ERR_NB    2
#define DEBUG_ACT_NB    3
#define DEBUG_COL_NB    4
#define DEBUG_RATIO_NB  5
#define DEBUG_SPI_NB    6

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

#define LIM_CUR_MIN     100 //+1mA ~ +1bit
#define LIM_CUR_MAX     250 //450

//déclaration de pin
#define PWM_SERVO_0     2
#define PWM_SERVO_1     3
#define PWM_SERVO_2     4
#define PUMP_0          5
#define PUMP_1          6
#define PUMP_2          7
#define MULT_A          8
#define MULT_B          9
#define SS              10
#define MOSI            11
#define MISO            12
#define SCK             13
#define TCS_ENABLE      A2
#define TCS_S3          A0
#define TCS_S2          A1
#define RUPT_ARM        A3
#define TCS_ARM         A4
#define I_ARM_0         A5
#define I_ARM_1         A6
#define I_ARM_2         A7

//retour des fonctions
#define PUMP_EN_CHARGE  2
#define PUMP_A_VIDE     1
#define PUMP_ERREUR     0
#define WRONG_CHANNEL   255
#define RUPT_OFF        0
#define RUPT_ON         1
#define ECHEC           0
#define OK              1

//plage des servo moteurs
#define SERVO_0_MIN     700
#define SERVO_0_MAX     1600
#define SERVO_1_MIN     700
#define SERVO_1_MAX     1600
#define SERVO_2_MIN     700
#define SERVO_2_MAX     1600

#define TAILLE_SPI      5

//etat machine spi
#define SPI_IDLE        0
#define SPI_NUM_VAR     1
#define SPI_MSG_UTILE   2
#define SPI_CHECKSUM    3

//definition des varaibles pouvant être recu en spi
#define AUCUNE            0
#define PUMP_0_SPI        1
#define PUMP_1_SPI        2
#define PUMP_2_SPI        3
#define MOVE_0_SPI        4
#define MOVE_1_SPI        5
#define MOVE_2_SPI        6
#define MOVE_POS_0_SPI    7
#define MOVE_POS_1_SPI    8
#define MOVE_POS_2_SPI    9
#define COLOR_SPI_0       10
#define COLOR_SPI_1       11
#define COLOR_SPI_2       12
#define CURENT_SPI_0      13
#define CURENT_SPI_1      14
#define CURENT_SPI_2      15
#define CURENT_SPI_FULL_0 16
#define CURENT_SPI_FULL_1 17
#define CURENT_SPI_FULL_2 18
#define COLOR_SPI_FULL_0  19
#define COLOR_SPI_FULL_1  20
#define COLOR_SPI_FULL_2  21
#define SEUIL_R_BAS       22
#define SEUIL_R_HAUT      23
#define SEUIL_G_BAS       24
#define SEUIL_G_HAUT      25
#define SEUIL_B_BAS       26
#define SEUIL_B_HAUT      27
#define SEUIL_W           28
#define RESET_COLOR_ERR   29
#define MCLR              30

#define PUMP_LAUNCH_0   0
#define PUMP_LAUNCH_1   1
#define PUMP_LAUNCH_2   2

#define TAILLE_SEND     10

#define MAX_WAIT_COLOR  2

void Affiche(String txt, unsigned char state);
unsigned char MesureCourant(unsigned char nb_pump);
char GetColor(unsigned char nb_bras);
void StatePump(unsigned char nb_pump, boolean state);
void MoveServo(unsigned char nb_servo, int temps);
unsigned char GetRuptState(unsigned char nb_servo);
unsigned char RatioErr(unsigned char nb_bras, boolean minmax, int taille);
void Sequence(unsigned char nb_bras);
void(* resetFunc) (void) = 0; //declare reset function @ address 0

Servo Servo0;
Servo Servo1;
Servo Servo2;

//limite des couleurs 40-135(6)-420(1)-3000
int LIM_RED_MIN = 15;
int LIM_RED_MAX = 39;
int LIM_GREEN_MIN = 39;
int LIM_GREEN_MAX = 100;
int LIM_BLUE_MIN = 195;
int LIM_BLUE_MAX = 350;
int LIM_WHITE_MAX = 32;
int Hg_0 = 0;
int Hg_1 = 0;
int Hg_2 = 0;

//spi
char EtatSpi = SPI_IDLE;              //etat de la machine à état spi
short int Checksum;                   //checksum de récéption
short int ChecksumTemp;               //checksum d'émission
unsigned char TailleMsgSpi = 0;       //taille du message recu
unsigned char CptSpi = 0;             //compteur de d'octets recu
unsigned char TypeVarSpi = AUCUNE;    //type de variable recu
unsigned char TextSpi[TAILLE_SPI];    //message recu
unsigned char SendNbSpi = 0;          //message envoyer
unsigned char SendNbSpi_1 = 0;        //message envoyer 1
boolean FlagReceiveSpi = 0;           //drapeau de récéption d'un message spi
char TabPileSend[TAILLE_SEND];        //pile de récéption pour stocker les réponse à donner
int CptPile = 0;                      //compteur de la pile de remplissage
int CptReadPile = 0;                  //compteur de la pile pour la vider
int CptSpiSend = 0;                   //compteur des octets d'envoi

//loop
unsigned char Color0 = 0;
unsigned char Color1 = 0;
unsigned char Color2 = 0;
unsigned char Cur0 = 0;
unsigned char Cur1 = 0;
unsigned char Cur2 = 0;
int Cur_full_0 = 0;
int Cur_full_1 = 0;
int Cur_full_2 = 0;
boolean Rupt0 = 0;
boolean Rupt1 = 0;
boolean Rupt2 = 0;

//capteur couleur
int cpt_capt_0 = 0;
int cpt_capt_1 = 0;
int cpt_capt_2 = 0;

int test = 0;

void setup() {
  Serial.begin(115200);

  Affiche("START INIT", DEBUG_NORM_NB);
  
  pinMode(PUMP_0, OUTPUT);
  pinMode(PUMP_1, OUTPUT);
  pinMode(PUMP_2, OUTPUT);
  pinMode(PWM_SERVO_0, OUTPUT);
  pinMode(PWM_SERVO_1, OUTPUT);
  pinMode(PWM_SERVO_2, OUTPUT);
  pinMode(MULT_A, OUTPUT);
  pinMode(MULT_B, OUTPUT);
  pinMode(SS, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(SCK, INPUT);
  pinMode(TCS_ENABLE, OUTPUT);
  pinMode(TCS_S2, OUTPUT);
  pinMode(TCS_S3, OUTPUT);
  pinMode(RUPT_ARM, INPUT);
  pinMode(TCS_ARM, INPUT);
    
  digitalWrite(TCS_ENABLE, LOW); //enable tcs230
  digitalWrite(PUMP_0, LOW);
  digitalWrite(PUMP_1, LOW);
  digitalWrite(PUMP_2, LOW);
  Servo0.attach(PWM_SERVO_0, SERVO_0_MIN, SERVO_0_MAX);
  Servo1.attach(PWM_SERVO_1, SERVO_1_MIN, SERVO_1_MAX);
  Servo2.attach(PWM_SERVO_2, SERVO_2_MIN, SERVO_2_MAX);

  //spi
  SPCR = _BV(SPE);    // turn on SPI in slave mode
  SPCR |= _BV(SPIE);  // turn on interrupts
  SPDR = 0;           //clear buffer SPI

  int i;
  for(i=0;i<TAILLE_SEND;i++) {
    TabPileSend[i] = AUCUNE;
  }

  MoveServo(0, SERVO_0_MIN);
  MoveServo(1, SERVO_1_MIN);
  MoveServo(2, SERVO_2_MIN);
  
  Affiche("FIN INIT", DEBUG_NORM_NB);
}

void loop() {
  //lecture continu de la couleur et du courant
  Color0 = RatioErr(PUMP_LAUNCH_0, 0, 10);
  Affiche("Color0 = "+String(Color0), DEBUG_NORM_NB);
  Color1 = RatioErr(PUMP_LAUNCH_1, 0, 10);
  Affiche("Color1 = "+String(Color1), DEBUG_NORM_NB);
  Color2 = RatioErr(PUMP_LAUNCH_2, 0, 10);
  Affiche("Color2 = "+String(Color2), DEBUG_NORM_NB);
  Cur0 = MesureCourant(PUMP_LAUNCH_0);
  Cur1 = MesureCourant(PUMP_LAUNCH_1);
  Cur2 = MesureCourant(PUMP_LAUNCH_2);
  /*Rupt0 = GetRuptState(PUMP_LAUNCH_0);
  Rupt1 = GetRuptState(PUMP_LAUNCH_1);
  Rupt2 = GetRuptState(PUMP_LAUNCH_2);*/
  //affichage des tram recu en spi
  if(FlagReceiveSpi) {
    FlagReceiveSpi = 0;
    Affiche("Taille = "+String((int)(TailleMsgSpi)), DEBUG_SPI_NB);
    Affiche("Type = "+String((int)(TypeVarSpi)), DEBUG_SPI_NB);
    int i;
    for(i=0;i<TailleMsgSpi-2;i++) {
      Affiche("msg = "+String((int)(TextSpi[i])), DEBUG_SPI_NB);
    }
    Affiche("Checksum = "+String((int)(Checksum/2)), DEBUG_SPI_NB);
  }
  /*digitalWrite(MULT_A, 1);
  delay(500);
  digitalWrite(MULT_A, 0);
  delay(500);*/
  /*int br = 2;
  //MoveServo(br, SERVO_0_MAX);
  StatePump(br, 1);
  delay(100);
  MoveServo(br, SERVO_0_MIN);
  //MesureCourant(br);
  StatePump(br, 0);
  delay(100);*/
  //MesureCourant(br);
}

/*******************************************************************/
// Nom de la fonction: GetColor
//                    
// Description: Fonction renvoi la couleur vu par le capteur 0=erreur, 1=rouge, 2=vert, 3=bleu
//
// Entrées: unsigned char nb_bras : numéro du bras
//
// Sorties: unsigned char col : nombre renvoyé
//
//                              
/*******************************************************************/
char GetColor(unsigned char nb_bras) {
  Affiche("bras = "+String(nb_bras), DEBUG_COL_NB);
  int period, red, green, blue;
  double H, C, m, M, white;
  unsigned char col;
  switch(nb_bras) {
    case 0:
      digitalWrite(MULT_A, LOW);
      digitalWrite(MULT_B, LOW);
    break;
    case 1:
      digitalWrite(MULT_A, HIGH);
      digitalWrite(MULT_B, LOW);
    break;
    case 2:
      digitalWrite(MULT_A, LOW);
      digitalWrite(MULT_B, HIGH);
    break;
    default:
      Affiche("WRONG_CHANNEL", DEBUG_ERR_NB);
      return WRONG_CHANNEL;
    break;
  }
  if(cpt_capt_0 < MAX_WAIT_COLOR && nb_bras == 0 || cpt_capt_1 < MAX_WAIT_COLOR && nb_bras == 1 || cpt_capt_2 < MAX_WAIT_COLOR && nb_bras == 2) {

    digitalWrite(TCS_ENABLE, LOW);
    // temps d'exec : 400-800us
  
    // Lecture de la composante rouge
    digitalWrite(TCS_S2,LOW);
    digitalWrite(TCS_S3,LOW);
    delayMicroseconds(10);
    period = pulseIn(TCS_ARM, LOW);
    if(MAP_RED_MIN >= period) {
      Affiche("erreur lecture basse rouge"+String(period), DEBUG_ERR_NB);
      switch(nb_bras) {
        case 0:
          cpt_capt_0++;
        break;
        case 1:
          cpt_capt_1++;
        break;
        case 2:
          cpt_capt_2++;
        break;
      }
    } else {
      cpt_capt_0 = 0;
      cpt_capt_1 = 0;
      cpt_capt_2 = 0;
      
      if(MAP_RED_MAX <= period) {Affiche("erreur lecture haute rouge"+String(period), DEBUG_ERR_NB);}
      Affiche("Pr="+String(period), DEBUG_COL_NB);
      red = map(period, MAP_RED_MIN, MAP_RED_MAX,MAX_RGB,0);
  
      // Lecture de la composante verte
      digitalWrite(TCS_S2,HIGH);
      digitalWrite(TCS_S3,HIGH);
      delayMicroseconds(10);
      period = pulseIn(TCS_ARM, LOW);
      if(MAP_GREEN_MIN >= period) {Affiche("erreur lecture basse vert"+String(period), DEBUG_ERR_NB);}
      if(MAP_GREEN_MAX <= period) {Affiche("erreur lecture haute vert"+String(period), DEBUG_ERR_NB);}
      Affiche("Pg="+String(period), DEBUG_COL_NB);
      green = map(period, MAP_GREEN_MIN,MAP_GREEN_MAX,MAX_RGB,0);
  
      // Lecture de la composante bleue
      digitalWrite(TCS_S2,LOW);
      digitalWrite(TCS_S3,HIGH);
      delayMicroseconds(10);
      period = pulseIn(TCS_ARM, LOW);
      if(MAP_BLUE_MIN >= period) {Affiche("erreur lecture basse bleu"+String(period), DEBUG_ERR_NB);}
      if(MAP_BLUE_MAX <= period) {Affiche("erreur lecture haute bleu"+String(period), DEBUG_ERR_NB);}
      Affiche("Pb="+String(period), DEBUG_COL_NB);
      blue = map(period, MAP_BLUE_MIN,MAP_BLUE_MAX,MAX_RGB,0);

      // Lecture de la composante blanche
      digitalWrite(TCS_S2,HIGH);
      digitalWrite(TCS_S3,LOW);
      delayMicroseconds(10);
      period = pulseIn(TCS_ARM, LOW);
      if(MAP_WHITE_MIN >= period) {Affiche("erreur lecture basse blanche"+String(period), DEBUG_ERR_NB);}
      if(MAP_WHITE_MAX <= period) {Affiche("erreur lecture haute blanche"+String(period), DEBUG_ERR_NB);}
      Affiche("Pwh="+String(period), DEBUG_COL_NB);
      white = period;
      //adaptation de la luminosité
      white = -0.2*white+45; //test avec 40 ou 30 (45)
  
      Affiche("R="+String(red), DEBUG_COL_NB);
      Affiche("G="+String(green), DEBUG_COL_NB);
      Affiche("B="+String(blue), DEBUG_COL_NB);
      Affiche("W="+String(white), DEBUG_COL_NB);
      Affiche("", DEBUG_COL_NB);

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
        Affiche("erreur calcul", DEBUG_ERR_NB);
      }
      H *= 60;
      if(H < 0) {H *= -1;}
      #ifdef CORRECTION_LUM
        H += white;
      #endif
      Affiche("H="+String(H), DEBUG_COL_NB);
      // Déduction de la couleur à partir du HSL 
      switch(nb_bras) {
        case 0:
          Hg_0 = H;
        break;
        case 1:
          Hg_1 = H;
        break;
        case 2:
          Hg_2 = H;
        break;
      }
      if(white > LIM_WHITE_MAX) {
        col = 4;
        Affiche("pas de palet", DEBUG_COL_NB);
      } else if(LIM_RED_MIN < H && H < LIM_RED_MAX) {
        Affiche("Rouge", DEBUG_COL_NB);
        col = 1;
      } else if(LIM_GREEN_MIN < H && H < LIM_GREEN_MAX) {
        Affiche("Vert", DEBUG_COL_NB);
        col = 2;
      } else if(LIM_BLUE_MIN < H && H < LIM_BLUE_MAX) {
        Affiche("Bleu", DEBUG_COL_NB);
        col = 3;
      } else {
        Affiche("Aucune couleur", DEBUG_COL_NB);
        col = 0;
      }
      //digitalWrite(TCS_ENABLE, HIGH);
      return col;
    }
  } else {
    return 5;
  }
}

/*******************************************************************/
// Nom de la fonction: StatePump
//                    
// Description: Fonction qui allume la pompe ou l'éteint
//
// Entrées: unsigned char nb_pump : numéro du bras
//
//          boolean state : etat a affecter
//
//                              
/*******************************************************************/
void StatePump(unsigned char nb_pump, boolean state) {
  switch(nb_pump) {
    case 0:
      digitalWrite(PUMP_0, state);
    break;
    case 1:
      digitalWrite(PUMP_1, state);
    break;
    case 2:
      digitalWrite(PUMP_2, state);
    break;
    default:
      Affiche("WRONG_CHANNEL", DEBUG_ERR_NB);
    break;
  }
  Affiche("PUMP = "+String(nb_pump,DEC), DEBUG_ACT_NB);
  Affiche("STATE = "+String(state,DEC), DEBUG_ACT_NB);
}

/*******************************************************************/
// Nom de la fonction: MoveServo
//                    
// Description: Fonction qui allume la pompe ou l'éteint
//
// Entrées: unsigned char nb_servo : numéro du bras
//
//          int temps : temps qui fixe la position du moteur
//
//                              
/*******************************************************************/
void MoveServo(unsigned char nb_servo, int temps) {
  switch(nb_servo) {
    case 0:
      Servo0.writeMicroseconds(temps);
    break;
    case 1:
      Servo1.writeMicroseconds(temps);
    break;
    case 2:
      Servo2.writeMicroseconds(temps);
    break;
    default:
      Affiche("WRONG_CHANNEL", DEBUG_ERR_NB);
    break;
  }
  Affiche("Servo = "+String(nb_servo,DEC), DEBUG_ACT_NB);
  Affiche("Temps = "+String(temps,DEC), DEBUG_ACT_NB);
}

/*******************************************************************/
// Nom de la fonction: GetRuptState
//                    
// Description: Fonction qui renvoi l'état du rupteur
//
// Entrées: unsigned char nb_servo : numéro du bras
//
// Sorties: unsigned char res : nombre renvoyé
//                              
/*******************************************************************/
unsigned char GetRuptState(unsigned char nb_servo) {
  unsigned char res;
  switch(nb_servo) {
    case 0:
      digitalWrite(MULT_A, LOW);
      digitalWrite(MULT_B, LOW);
      
    break;
    case 1:
      digitalWrite(MULT_A, HIGH);
      digitalWrite(MULT_B, LOW);
    break;
    case 2:
      digitalWrite(MULT_A, LOW);
      digitalWrite(MULT_B, HIGH);
    break;
    default:
      Affiche("WRONG_CHANNEL", DEBUG_ERR_NB);
      return WRONG_CHANNEL;
    break;
  }
  if(digitalRead(RUPT_ARM)) {
    res = RUPT_ON;
  } else {
    res = RUPT_OFF;
  }
  Affiche("RUPT = "+String(nb_servo,DEC), DEBUG_ACT_NB);
  Affiche("STATE = "+String(res,DEC), DEBUG_ACT_NB);
  return res;
}

/*******************************************************************/
// Nom de la fonction: MesureCourant
//                    
// Description: Fonction qui renvoi l'état du courant dans la pompe
//
// Entrées: unsigned char nb_pump : numéro du bras
//
// Sorties: unsigned char res : nombre renvoyé PUMP_EN_CHARGE => courant > 512, PUMP_A_VIDE => courant > 100, sinon PUMP_ERREUR
//                              
/*******************************************************************/
unsigned char MesureCourant(unsigned char nb_pump) {
  int courant;
  unsigned char res;
  switch(nb_pump) {
    case 0:
      courant = analogRead(I_ARM_0);
      Cur_full_0 = courant;
    break;
    case 1:
       courant = analogRead(I_ARM_1);
      Cur_full_1 = courant;
    break;
    case 2:
       courant = analogRead(I_ARM_2);
      Cur_full_2 = courant;
    break;
    default:
      Affiche("current error", DEBUG_ERR_NB);
      return WRONG_CHANNEL;
    break;
  }
  Affiche(String(courant), DEBUG_ACT_NB);
  if(courant > LIM_CUR_MAX) {
    res = PUMP_EN_CHARGE;
  } else {
    if(courant < LIM_CUR_MIN && digitalRead(PUMP_0) && nb_pump == PUMP_LAUNCH_0 || courant < LIM_CUR_MIN && digitalRead(PUMP_1) && nb_pump == PUMP_LAUNCH_1 || courant < LIM_CUR_MIN && digitalRead(PUMP_2) && nb_pump == PUMP_LAUNCH_2) {
      res = PUMP_ERREUR;
      Affiche("current error", DEBUG_ERR_NB);
    } else {
      res = PUMP_A_VIDE;
    }
  }
  Affiche("RES = "+String(res), DEBUG_ACT_NB);
  Affiche("PUMP = "+String(nb_pump), DEBUG_ACT_NB);
  Affiche("COURANT = "+String(courant), DEBUG_ACT_NB);
  return res;
}

/*******************************************************************/
// Nom de la fonction: RatioErr
//                    
// Description: Fonction qui renvoi une moyenne ou les valeurs min et max des couleurs lu
//
// Entrées: unsigned char nb_bras : numéro du bras scanner
//
//          boolean minmax : 1=renvoi min et max, 0 renvoi moyenne
//
//          int taille : la taille de l'échantillon
//
// Sorties : -1=minmax, 1=rouge, 2=vert, 3=bleu
//                              
/*******************************************************************/
unsigned char RatioErr(unsigned char nb_bras, boolean minmax, int taille) {
  int i, R = 0, G = 0, B = 0, E = 0, C = 0, W = 0, test;
  double mini = GetColor(nb_bras), maxi = GetColor(nb_bras);
  Affiche("Bras = "+String(nb_bras), DEBUG_RATIO_NB);
  for(i=0;i<taille;i++) {
    test = GetColor(nb_bras);
    if(mini > test) {mini = test;}
    if(maxi < test) {maxi = test;}
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
  if(minmax) {
    //pb dans la fonction
    Affiche(String(mini), DEBUG_RATIO_NB);
    Affiche(String(maxi), DEBUG_RATIO_NB);
    Affiche("", DEBUG_RATIO_NB);
    return -1;
  } else {
    Affiche("Ratio Red = "+String(R), DEBUG_RATIO_NB);
    Affiche("Ratio Blue = "+String(B), DEBUG_RATIO_NB);
    Affiche("Ratio Green = "+String(G), DEBUG_RATIO_NB);
    Affiche("Ratio Erreur = "+String(E), DEBUG_RATIO_NB);
    Affiche("", DEBUG_RATIO_NB);
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
}

/*******************************************************************/
// Nom de la fonction: Sequence
//                    
// Description: Fonction qui effectu toute la sequence pour chercher un palet sur la bras entrée
//
// Entrées: unsigned char nb_bras : bras sélectionné
//                              
/*******************************************************************/
void Sequence(unsigned char nb_bras) {
  MoveServo(nb_bras, SERVO_0_MAX);
  delay(2000);
  //GetColor(nb_bras);
  switch(RatioErr(nb_bras, 0, 10)) {
    case 1:
      Affiche("rouge", DEBUG_ACT_NB);
      test = 1;
    break;
    case 2:
      Affiche("vert", DEBUG_ACT_NB);
      test = 2;
    break;
    case 3:
      Affiche("bleu", DEBUG_ACT_NB);
      test = 3;
    break;
    default:
      Affiche("err", DEBUG_ACT_NB);
      test = 0;
    break;
  }
  if(nb_bras+1 == test) {
    delay(200);
    StatePump(nb_bras, 1);
    delay(1000);
    MoveServo(nb_bras, SERVO_0_MIN);
    delay(5000);
    MoveServo(nb_bras, 1500);
    delay(1000);
    StatePump(nb_bras, 0);
    delay(2000);
  }
  MoveServo(nb_bras, SERVO_0_MIN);
  delay(2000);
}

/*******************************************************************/
// Nom de la fonction: Affiche
//                    
// Description: Fonction qui effectu un affichage si DEBUG est déclaré
//
// Entrées: String txt : chaine à afficher
//                              
/*******************************************************************/
void Affiche(String txt, unsigned char state) {
  #ifdef DEBUG_NORM
    if(state == DEBUG_NORM_NB) {
      Serial.println(txt);
    }
  #endif
  #ifdef DEBUG_ERR
    if(state == DEBUG_ERR_NB) {
      Serial.println(txt);
    }
  #endif
  #ifdef DEBUG_ACT
    if(state == DEBUG_ACT_NB) {
      Serial.println(txt);
    }
  #endif
  #ifdef DEBUG_COL
    if(state == DEBUG_COL_NB) {
      Serial.println(txt);
    }
  #endif
  #ifdef DEBUG_SPI
    if(state == DEBUG_SPI_NB) {
      Serial.println(txt);
    }
  #endif
  #ifdef DEBUG_RATIO
    if(state == DEBUG_RATIO_NB) {
      Serial.println(txt);
    }
  #endif
}

/*-------------- SPI INTERRUPT --------------*/
ISR(SPI_STC_vect) {
  unsigned char data_spi = SPDR;
  switch(EtatSpi) {
    case SPI_IDLE:
      //gestion de la taille message
      TailleMsgSpi = data_spi;
      TypeVarSpi = AUCUNE;
      if(TailleMsgSpi) {
        EtatSpi = SPI_NUM_VAR;
      }
      Checksum = 0;
      CptSpi = 1;
    break;
    case SPI_NUM_VAR:
      //gestion du type
      TypeVarSpi = data_spi;
      //if(TypeVarSpi == COLOR_SPI_0 || TypeVarSpi == COLOR_SPI_1 || TypeVarSpi == COLOR_SPI_2 || TypeVarSpi == CURENT_SPI_0 || TypeVarSpi == CURENT_SPI_1 || TypeVarSpi == CURENT_SPI_2 || TypeVarSpi == CURENT_SPI_FULL_0 || TypeVarSpi == CURENT_SPI_FULL_1 || TypeVarSpi == CURENT_SPI_FULL_2 || TypeVarSpi == COLOR_SPI_FULL_0 || TypeVarSpi == COLOR_SPI_FULL_1 || TypeVarSpi == COLOR_SPI_FULL_2 || TypeVarSpi == RESET_COLOR_ERR) {
      if(TailleMsgSpi == 2) {
        EtatSpi = SPI_CHECKSUM; //pas de message utile
      } else {
        EtatSpi = SPI_MSG_UTILE;
      }
      CptSpi++;
    break;
    case SPI_MSG_UTILE:
      //gestion du message
      TextSpi[CptSpi-2] = data_spi;
      CptSpi++;
      if(CptSpi == TailleMsgSpi) {
        EtatSpi = SPI_CHECKSUM;
      }
    break;
    case SPI_CHECKSUM:
      //gestion du checksum
      if(Checksum%256 == data_spi) {
        FlagReceiveSpi = 1;
        switch(TypeVarSpi) {
          case PUMP_0_SPI:
            if(TextSpi[0]) {
              digitalWrite(PUMP_0, HIGH);
            } else {
              digitalWrite(PUMP_0, LOW);
            }
          break;
          case PUMP_1_SPI:
            if(TextSpi[0]) {
              digitalWrite(PUMP_1, HIGH);
            } else {
              digitalWrite(PUMP_1, LOW);
            }
          break;
          case PUMP_2_SPI:
            if(TextSpi[0]) {
              digitalWrite(PUMP_2, HIGH);
            } else {
              digitalWrite(PUMP_2, LOW);
            }
          break;
          case MOVE_0_SPI:
            if(TextSpi[0]) {
              Servo0.writeMicroseconds(SERVO_0_MAX);
            } else {
              Servo0.writeMicroseconds(SERVO_0_MIN);
            }
          break;
          case MOVE_1_SPI:
            if(TextSpi[0]) {
              Servo1.writeMicroseconds(SERVO_1_MAX);
            } else {
              Servo1.writeMicroseconds(SERVO_1_MIN);
            }
          break;
          case MOVE_2_SPI:
            if(TextSpi[0]) {
              Servo2.writeMicroseconds(SERVO_2_MAX);
            } else {
              Servo2.writeMicroseconds(SERVO_2_MIN);
            }
          break;
          case MOVE_POS_0_SPI:
            Servo0.writeMicroseconds(TextSpi[0]*256+TextSpi[1]);
          break;
          case MOVE_POS_1_SPI:
            Servo1.writeMicroseconds(TextSpi[0]*256+TextSpi[1]);
          break;
          case MOVE_POS_2_SPI:
            Servo2.writeMicroseconds(TextSpi[0]*256+TextSpi[1]);
          break;
          case COLOR_SPI_0:
            TabPileSend[CptPile] = COLOR_SPI_0;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case COLOR_SPI_1:
            TabPileSend[CptPile] = COLOR_SPI_1;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case COLOR_SPI_2:
            TabPileSend[CptPile] = COLOR_SPI_2;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case CURENT_SPI_0:
            TabPileSend[CptPile] = CURENT_SPI_0;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case CURENT_SPI_1:
            TabPileSend[CptPile] = CURENT_SPI_1;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case CURENT_SPI_2:
            TabPileSend[CptPile] = CURENT_SPI_2;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case CURENT_SPI_FULL_0:
            TabPileSend[CptPile] = CURENT_SPI_FULL_0;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case CURENT_SPI_FULL_1:
            TabPileSend[CptPile] = CURENT_SPI_FULL_1;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case CURENT_SPI_FULL_2:
            TabPileSend[CptPile] = CURENT_SPI_FULL_2;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case  COLOR_SPI_FULL_0:
            TabPileSend[CptPile] = COLOR_SPI_FULL_0;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case  COLOR_SPI_FULL_1:
            TabPileSend[CptPile] = COLOR_SPI_FULL_1;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case  COLOR_SPI_FULL_2:
            TabPileSend[CptPile] = COLOR_SPI_FULL_2;
            CptPile++;
            CptPile %= TAILLE_SEND;
          break;
          case SEUIL_R_BAS:
            LIM_RED_MIN = TextSpi[0]*256+TextSpi[1];
          break;
          case SEUIL_R_HAUT:
            LIM_RED_MAX = TextSpi[0]*256+TextSpi[1];
          break;
          case SEUIL_G_BAS:
            LIM_GREEN_MIN = TextSpi[0]*256+TextSpi[1];
          break;
          case SEUIL_G_HAUT:
            LIM_GREEN_MAX = TextSpi[0]*256+TextSpi[1];
          break;
          case SEUIL_B_BAS:
            LIM_BLUE_MIN = TextSpi[0]*256+TextSpi[1];
          break;
          case SEUIL_B_HAUT:
            LIM_BLUE_MAX = TextSpi[0]*256+TextSpi[1];
          break;
          case SEUIL_W:
            LIM_WHITE_MAX = TextSpi[0]*256+TextSpi[1];
          break;
          case RESET_COLOR_ERR:
            cpt_capt_0 = 0;
            cpt_capt_1 = 0;
            cpt_capt_2 = 0;
          break;
          case MCLR:
            resetFunc();  //call reset
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
  //parti de reponse
  if(TabPileSend[CptReadPile]) {
    CptSpiSend++;
    switch(CptSpiSend) {
      case 1:
        if(CURENT_SPI_FULL_0 == TabPileSend[CptReadPile] || CURENT_SPI_FULL_1 == TabPileSend[CptReadPile] || CURENT_SPI_FULL_2 == TabPileSend[CptReadPile] || COLOR_SPI_FULL_0 == TabPileSend[CptReadPile] || COLOR_SPI_FULL_1 == TabPileSend[CptReadPile] || COLOR_SPI_FULL_2 == TabPileSend[CptReadPile]) {
          SPDR = 0x04;
        } else {
          SPDR = 0x03;
        }
      break;
      case 2:
        SPDR = TabPileSend[CptReadPile];
      break;
      case 3:
        switch(TabPileSend[CptReadPile]) {
          case COLOR_SPI_0:
            SendNbSpi = Color0;
            CptSpiSend++;
          break;
          case COLOR_SPI_1:
            SendNbSpi = Color1;
            CptSpiSend++;
          break;
          case COLOR_SPI_2:
            SendNbSpi = Color2;
            CptSpiSend++;
          break;
          case COLOR_SPI_FULL_0:
            SendNbSpi = (unsigned char)(Hg_0/256);
          break;
          case COLOR_SPI_FULL_1:
            SendNbSpi = (unsigned char)(Hg_1/256);
          break;
          case COLOR_SPI_FULL_2:
            SendNbSpi = (unsigned char)(Hg_2/256);
          break;
          case CURENT_SPI_0:
            SendNbSpi = Cur0;
            CptSpiSend++;
          break;
          case CURENT_SPI_1:
            SendNbSpi = Cur1;
            CptSpiSend++;
          break;
          case CURENT_SPI_2:
            SendNbSpi = Cur2;
            CptSpiSend++;
          break;
          case CURENT_SPI_FULL_0:
            SendNbSpi = (unsigned char)(Cur_full_0/256);
          break;
          case CURENT_SPI_FULL_1:
            SendNbSpi = (unsigned char)(Cur_full_1/256);
          break;
          case CURENT_SPI_FULL_2:
            SendNbSpi = (unsigned char)(Cur_full_2/256);
          break;
        }
        SPDR = SendNbSpi;
      break;
      case 4:
        switch(TabPileSend[CptReadPile]) {
          case CURENT_SPI_FULL_0:
            SendNbSpi_1 = (Cur_full_0%256);
          break;
          case CURENT_SPI_FULL_1:
            SendNbSpi_1 = (Cur_full_1%256);
          break;
          case CURENT_SPI_FULL_2:
            SendNbSpi_1 = (Cur_full_2%256);
          break;
          case COLOR_SPI_FULL_0:
            SendNbSpi_1 = (Hg_0%256);
          break;
          case COLOR_SPI_FULL_1:
            SendNbSpi_1 = (Hg_1%256);
          break;
          case COLOR_SPI_FULL_2:
            SendNbSpi_1 = (Hg_2%256);
          break;
        }
        SPDR = SendNbSpi_1;
      break;
      case 5:
        if(CURENT_SPI_FULL_0 == TabPileSend[CptReadPile] || CURENT_SPI_FULL_1 == TabPileSend[CptReadPile] || CURENT_SPI_FULL_2 == TabPileSend[CptReadPile] || COLOR_SPI_FULL_0 == TabPileSend[CptReadPile] || COLOR_SPI_FULL_1 == TabPileSend[CptReadPile] || COLOR_SPI_FULL_2 == TabPileSend[CptReadPile]) {
          SPDR = (0x04 + TabPileSend[CptReadPile] + SendNbSpi + SendNbSpi_1)%256;
        } else {
          SPDR = (0x03 + TabPileSend[CptReadPile] + SendNbSpi)%256;
        }
        TabPileSend[CptReadPile] = AUCUNE;
        CptReadPile++;
        CptReadPile %= TAILLE_SEND;
        CptSpiSend = 0;
        SendNbSpi = 0;
        SendNbSpi_1 = 0;
      break;
    }
  } else {
    SPDR = 0;
  }
}
