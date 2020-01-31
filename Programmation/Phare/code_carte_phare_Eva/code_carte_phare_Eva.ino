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

char receive;
 
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
  digitalWrite(state, 1);
  digitalWrite(RESET_XBEE, 1);
  digitalWrite(sens_act_Montee, 0);
  digitalWrite(sens_act_Tourner, 0);
  digitalWrite(leds, 0);
  analogWrite(pwm_act_Monter, 0);
  analogWrite(pwm_act_Tourner, 0);
  Serial.begin(300);
  /*digitalWrite(sens_act_Montee, descendrePhare);
  analogWrite(pwm_act_Monter, vitesseDeCroisiereMonter);
  while(!digitalRead(RUPT_INFO_Bas)) {}
  analogWrite(pwm_act_Monter, 0);*/
}

void loop() {
  Serial.println("bonjour");
  /*if(Serial.available()) {
    receive = Serial.read();
    if(receive == 'A') {
      digitalWrite(leds, 1);
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
        if(Serial.available()) {
          receive = Serial.read();
          if(receive == 'E') {
              
          }
        }
      }
    }
  }*/
}
