#include <Arduino.h>
#include <U8g2lib.h>
#include <Servo.h>
#define button 10
#define PIN_TRIG 2
#define PIN_ECHO 3
#define PIN_TRIGB 4
#define PIN_ECHOB 5
#define buzzer 6
#define led_Red 8
#define led_Green 7

Servo myservo;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int etape = 0;
unsigned long depart = 0;
unsigned long derniereMesure = 0;
long distance = 100;   // 100 = "rien devant" au démarrage
unsigned long derniereMesureB = 0;
long distanceB = 100;
bool dejaDit = false;
bool dejaDitB = false;

void setup() {
  myservo.attach(9);
  pinMode(button, INPUT_PULLUP);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_TRIGB, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_ECHOB, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led_Red, OUTPUT);
  pinMode(led_Green, OUTPUT);
  myservo.write(0);
  Serial.begin(9600);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB18_tr);
  u8g2.drawStr(10, 30, "Bonjour !");
  u8g2.sendBuffer();
}

void loop() {
  unsigned long actuel = millis();

  // --- Mesure de distance, une fois toutes les 60 ms ---
  if (actuel - derniereMesure >= 60) {
    derniereMesure = actuel;

    digitalWrite(PIN_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    long dure = pulseIn(PIN_ECHO, HIGH, 30000);  // timeout 30 ms
    if (dure > 0) {                              // on ignore les mesures ratées
      long d = (dure * 0.034) / 2;
      distance = constrain(d, 2, 100);
    }
    Serial.println("distance");
    Serial.println(distance);   // à retirer une fois que tout marche
  }
  if (actuel - derniereMesureB >= 60) {
    derniereMesureB = actuel;

    digitalWrite(PIN_TRIGB, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_TRIGB, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIGB, LOW);

    long dureB = pulseIn(PIN_ECHOB, HIGH, 30000);  // timeout 30 ms
    if (dureB > 0) {                              // on ignore les mesures ratées
      long dB = (dureB * 0.034) / 2;
      distanceB = constrain(dB, 2, 100);
    }
    Serial.println("distance B");
    Serial.println(distanceB);   // à retirer une fois que tout marche
  }

  // --- Machine à états ---
  if (etape == 0) {
    digitalWrite(led_Red, HIGH);
    digitalWrite(led_Green, LOW);
      if(!dejaDitB){
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB18_tr);
        u8g2.drawStr(10, 30, "Bonjour !");
        u8g2.sendBuffer();
        dejaDitB = true;
      }
    // Attente du bouton
    if (digitalRead(button) == LOW) {
      tone(buzzer, 1175, 300);
      myservo.write(50);
      depart = actuel;
      Serial.println("Bouton appuyé, barrière ouverte");
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_helvB12_tr);
      u8g2.drawStr(10, 30, "Barrière ouverte.");
      u8g2.sendBuffer();
      digitalWrite(led_Red, LOW);
      digitalWrite(led_Green, HIGH);
      etape = 1;
    }

  } else if (etape == 1) {
    if(distance < 9){
      depart = actuel;
      etape = 2;
    }
  }
    else if (etape == 2){
      if(distance < 9){
        if(actuel - depart >= 500){
          depart = actuel;
          etape = 3;
      }
      } else {
        etape = 1;
      }

  }
    else if(etape == 3){
    if (actuel - depart >= 500) {
      if (distance >= 9 && distanceB > 4) {
        myservo.write(0);
        Serial.println("Barrière fermée");
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB18_tr);
        u8g2.drawStr(30, 60, "Merci !");
        u8g2.sendBuffer();
        digitalWrite(led_Red, HIGH);
      digitalWrite(led_Green, LOW);
        dejaDit = false;
        dejaDitB = false;
        delay(500);
        etape = 0;
      } else {
        Serial.println("Obstacle");
        if(!dejaDit){
          u8g2.clearBuffer();
          u8g2.setFont(u8g2_font_helvB12_tr);
          u8g2.drawStr(10, 30, "Veuillez passer");
          u8g2.sendBuffer();
          dejaDit = true;
        }
        etape = 4;
      }
    }
    } else if (etape == 4) {
    // Obstacle présent : on attend qu'il parte
    if (distance >= 9 && distanceB > 4) {
      depart = actuel;   // le chrono démarre ici, une seule fois
      etape = 5;
    }

  } else if (etape == 5) {
      myservo.write(0);
      Serial.println("Merci !");
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_helvB18_tr);
      u8g2.drawStr(10, 30, "Merci !");
      u8g2.sendBuffer();
      digitalWrite(led_Red, HIGH);
      digitalWrite(led_Green, LOW);
      dejaDit = false;
      dejaDitB = false;
      delay(500);
      etape = 0;
    
  }
}