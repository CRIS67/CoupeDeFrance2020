#define state             2
#define RUPT_INFO_Haut    3
#define RUPT_INFO_Bas     4
#define RUPT_INFO_DG      5
#define RESET_XBEE        6
#define PWM_Servo0        7
#define leds              8
#define pwm_servo1        9
#define sens_act_Montee   16
#define sens_act_Tourner  17 
#define pwm_act_Monter     18
#define pwm_act_Tourner    19


#define monterPhare                   1
#define descendrePhare                !monterPhare
#define tournerLedsGauche             0
#define tournerLedsDroite             !tournerLedsGauche
#define vitesseDeCroisiereMonter      200
#define vitesseDeCroisiereTourner     200

#include <NeoSWSerial.h>

int XBee_Adress = 1;              // Adresse propre de la XBee entre 0 et 255
int XBee_Adress_To_Send = 2;
String XBee_Team = "1";           // Adresse de la team de la XBee
String Other_Team = "2";          // Adresse de la team à laquelle les messages seront envoyés. Ces deux adresses se composent d'un unique caractère au choix
String Network_Adress = "1234";   // Adresse du réseau

const byte Rx_Pin = 0;            // Le Rx et Tx sont déplacés sur d'autres pins grâce à NeoSerial
const byte Tx_Pin = 1;

const boolean Debug = false;        // Affiche sur le moniteur série les adresses après configuration

char Flag = 'c';                  // Caractère indiquant le début d'un message

NeoSWSerial XBee(Rx_Pin, Tx_Pin);

String Data_Text = "";            // Chaine contenant le dernier message reçu
int Data_Type = 0;                // Type du dernier message reçu
int Data_Sender = 0;              // Adresse proprede la XBee qui à envoyé ledit message
boolean Data_Reading = false;
 
void setup() {
  pinMode(state, OUTPUT);
  pinMode(RUPT_INFO_Haut, INPUT);
  pinMode(RUPT_INFO_Bas, INPUT);
  pinMode(RUPT_INFO_DG, INPUT);
  pinMode(RESET_XBEE, OUTPUT);
  pinMode(leds, OUTPUT);
  pinMode(sens_act_Montee, OUTPUT);
  pinMode(sens_act_Tourner, OUTPUT);
  pinMode(pwm_act_Monter, OUTPUT);
  pinMode(pwm_act_Tourner, OUTPUT);
  digitalWrite(state, 0);
  digitalWrite(RESET_XBEE, 1);
  digitalWrite(sens_act_Montee, 0);
  digitalWrite(sens_act_Tourner, 0);
  digitalWrite(leds, 0);
  analogWrite(pwm_act_Monter, 0);
  analogWrite(pwm_act_Tourner, 0);
  XBee.begin(9600);               // et celui pour la communication via les XBees
  XBee_Config(XBee_Team, Other_Team, Network_Adress); // Configure la XBee avec les adresses spécifiées plus haut
  XBee.attachInterrupt(XBee_Receive);                 // La reception des messages fonctionne par interruption
  /*digitalWrite(sens_act_Montee, descendrePhare);
  analogWrite(pwm_act_Monter, vitesseDeCroisiereMonter);
  while(!digitalRead(RUPT_INFO_Bas)) {}
  analogWrite(pwm_act_Monter, 0);*/
}

void loop() {
  XBee_Send (XBee_Adress_To_Send, 3, "Moi Phare, toi Robot");
  if(Data_Text != "") {
    if(Data_Text == 'A') {
      Data_Clear();
      digitalWrite(leds, 1);
      digitalWrite(state,1);
      digitalWrite(sens_act_Montee, monterPhare);
      digitalWrite(sens_act_Tourner, tournerLedsGauche);
      analogWrite(pwm_act_Monter, vitesseDeCroisiereMonter);
      analogWrite(pwm_act_Tourner, vitesseDeCroisiereTourner);
      while(1) {
        if(digitalRead(RUPT_INFO_DG)) {
          digitalWrite(sens_act_Tourner, !digitalRead(sens_act_Tourner));
        }
        if(digitalRead(RUPT_INFO_Haut)) {
           analogWrite(pwm_act_Monter, 0);
        }
        if(Data_Text != "") {
          if(Data_Text == 'E') {
              
          }
        }
      }
    }
  }
}

static void XBee_Config(String Sender, String Receiver, String Network) { // Configure la XBee. Le programme reste bloqué si il ne détecte pas la XBee
  char thisByte = 0;
  while (XBee.available() > 0) {
    if (thisByte != '\r') {
      thisByte = XBee.read();
    }
  }
  XBee.print("ATRE\r");
  XBee.print("ATMY" + Sender + "\r");
  XBee.print("ATDL" + Receiver + "\r");
  XBee.print("ATID" + Network + "\r");
}

static void XBee_Send(int adress, int type, String data0) {   // Envoit un message

  int n = data0.length();
  String data;
  data = String(Flag) + char(adress) + char(XBee_Adress) + char(type) + char(n)  + data0;
  int chk = data.length();
  data = data + char(chk);
  XBee.print(data);

}

static void XBee_Receive( uint8_t c) {    // Programme d'interuption permettant de gérer la réception

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

static void Data_Clear() {      // Supprime les données reçues

  Data_Text = "";
  Data_Type = 0;
  Data_Sender = 0;

}

