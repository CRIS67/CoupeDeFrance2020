#include <NeoSWSerial.h>

int XBee_Adress = 2;              // Adresse propre de la XBee entre 0 et 255
int XBee_Adress_To_Send = 1;
String XBee_Team = "2";           // Adresse de la team de la XBee
String Other_Team = "1";          // Adresse de la team à laquelle les messages seront envoyés. Ces deux adresses se composent d'un unique caractère au choix
String Network_Adress = "1234";   // Adresse du réseau

const byte Rx_Pin = 0;            // Le Rx et Tx sont déplacés sur d'autres pins grâce à NeoSerial
const byte Tx_Pin = 1;

const boolean Debug = true;        // Affiche sur le moniteur série les adresses après configuration

char Flag = 'c';                  // Caractère indiquant le début d'un message

NeoSWSerial XBee(Rx_Pin, Tx_Pin);

String Data_Text = "";            // Chaine contenant le dernier message reçu
int Data_Type = 0;                // Type du dernier message reçu
int Data_Sender = 0;              // Adresse proprede la XBee qui à envoyé ledit message
boolean Data_Reading = false;

void setup() {

  if (Debug)
    Serial.begin(9600);             // Deux serial sont initialisés : celui pour la communication USB

  XBee.begin(9600);               // et celui pour la communication via les XBees
  XBee_Config(XBee_Team, Other_Team, Network_Adress); // Configure la XBee avec les adresses spécifiées plus haut
  XBee.attachInterrupt(XBee_Receive);                 // La reception des messages fonctionne par interruption

}

void loop() {

  if (Serial.read() == '\n') {                        // Consigne de l'utilisateur (touche entrée)
    XBee_Send (XBee_Adress_To_Send, 3, "A");         // Envoit un message de type 3 à l'adresse 2. Le type doit être conpris entre 0 et 255
  }

  if (Data_Text != "") {                                                                                                            // Si message reçu
    Serial.println("La XBee n " + String(Data_Sender) + " a envoyee un message de type " + String(Data_Type) + " : " + Data_Text);  // Affichage des données reçues
    Data_Clear();                                                                                                                   // Les données ont été lues : on peut donc les supprimer (sans quoi elles seraient
  }                                                                                                                                 // de nouveau lues à la prochaine itération si aucun nouveau message n'a été reçu)
  delay(500);                     // La lécture fonctionnant par interruption, on peut cadencer la boucle principale à la vitesse voulue

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
  XBee.print("ATCN\r");
  if (Debug) {
    Serial.println("XBee Adress : " + String(XBee_Adress));
    Serial.println("Team ID : " + Sender);
    Serial.println("Receiving Team ID : " + Receiver);
    Serial.println("Network Adress : " + Network);
  }

}

/*
  Composition de la trame :

  Byte n°: Valeur :
  0           c (01100011) : début du message
  1           adresse du destinataire
  2           adresse de l'envoyeur
  3           type de message
  4           N : longueur du message
  5 -> N+4    message
  N+5         checksum

*/

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

