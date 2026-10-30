#include <Arduino.h>
#include <Servo.h>
#include <U8g2lib.h>

#define PIN_TRIG 8
#define PIN_ECHO 7
#define led 11
#define buzzer 12

Servo myservo;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);



void setup() {
  myservo.attach(9);
  u8g2.begin();
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  Serial.println("Système opérationel");
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_helvB18_tr);	// choose a suitable font
  u8g2.drawStr(10, 30,"Bonjour !");	// write something to the internal memory
  u8g2.setFont(u8g2_font_helvB10_tr);	// choose a suitable font
  u8g2.drawStr(15, 54, "avancez svp.");
  u8g2.sendBuffer();
  delay(3000);
  int angleActuel = myservo.read();
  if(angleActuel == 0){
    for(int d = 0; d < 95; d++){
          myservo.write(d);
          delay(17);
        }
      }
      else{
        for(int d = angleActuel; d < 95; d++){
          myservo.write(d);
          delay(17);
        }
      }

}

unsigned long actuel = 0;
long dure = 0;
long distance = 0;
int contrDistance = 0;

int etatSysteme = 0;
unsigned long chronoDetecton = 0;
unsigned long chronoOuverture = 0;

void detection(){
  actuel = millis();
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  // Récupérer la valeur (temps)
  dure = pulseIn(PIN_ECHO, HIGH, 30000);
  //Bien cadrer la distance 
  if(dure > 0){
   distance = (dure*0.034)/2;
  //Convertion en distance
  contrDistance = constrain(distance, 2, 100);
  }
}

void loop() {
  detection();
    if (etatSysteme == 0){
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_helvB18_tr);
      u8g2.drawStr(10, 30,"Bonjour !");
      u8g2.setFont(u8g2_font_helvB10_tr);
      u8g2.drawStr(15, 54, "avancez svp.");
      u8g2.sendBuffer();
      if (contrDistance <= 20){
        etatSysteme = 1;
        chronoDetecton = millis();
        Serial.println("Veuillez rester en place pour procéder à l'ouverture de la porte");
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB12_tr); 
        u8g2.drawStr(5, 40, "Ne bougez pas");
        u8g2.sendBuffer();
      }
    }
    else if(etatSysteme == 1){
      if(contrDistance <= 20){
        if(millis() - chronoDetecton >= 1000){
          etatSysteme = 2;
          digitalWrite(led, HIGH);
          tone(buzzer, 900, 500);
          for(int d = 95; d >= 0; d--){
            myservo.write(d);
            delay(17);
          }
        
          chronoOuverture = millis();
          Serial.println("Porte ouverte vous pouvez entrer");
          u8g2.clearBuffer();
          u8g2.setFont(u8g2_font_helvB18_tr);
          u8g2.drawStr(10, 40,"OUVERT !");
          u8g2.sendBuffer();
        }
      }
      else{
        etatSysteme = 0;
      }
}
    else if(etatSysteme == 2){
      if(millis() - chronoOuverture >= 6000){
        etatSysteme = 0;
        for(int d = 0; d < 95; d++){
          myservo.write(d);
          delay(17);
        }
        digitalWrite(led, LOW);
        Serial.println("Porte fermée");
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB18_tr);
        u8g2.drawStr(10, 40,"FERME !");
        u8g2.sendBuffer();
        delay(3000);
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB18_tr);
        u8g2.drawStr(10, 30,"Bonjour !");
        u8g2.setFont(u8g2_font_helvB10_tr);
        u8g2.drawStr(15, 54, "avancez svp.");
        u8g2.sendBuffer();
      }
    }
  }
