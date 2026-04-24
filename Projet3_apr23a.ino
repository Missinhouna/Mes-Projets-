#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Broches ESP32
const int LED_Rouge = 4;    // Évite GPIO 12
const int LED_Verte = 5;
const int BOUTON = 15;      // GPIO sûr pour entrée
const int BUZZER = 2;

unsigned long dernierChangementBuzzer = 0;
int etatBuzzer = LOW;          
bool cycleBuzzerActif = false;  

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Essayez 0x27 ou 0x3F

void setup() {
  Serial.begin(115200);  // ESP32 préfère 115200
  Serial.println("Démarrage ESP32...");
  
  pinMode(LED_Rouge, OUTPUT);
  pinMode(LED_Verte, OUTPUT);
  pinMode(BOUTON, INPUT_PULLUP);  // Fonctionne sur ESP32
  pinMode(BUZZER, OUTPUT);

  // Initialisation I2C pour ESP32 (broches par défaut: SDA=21, SCL=22)
  Wire.begin();  // Important pour ESP32!
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("ESP32 Test");
  delay(2000);
  
  digitalWrite(LED_Rouge, HIGH);  
  digitalWrite(LED_Verte, LOW);   
  digitalWrite(BUZZER, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LED Rouge ON");
  lcd.setCursor(0, 1);
  lcd.print("Verte OFF");
  
  Serial.println("Setup terminé");
}

void loop() {
  int etatBouton = digitalRead(BOUTON);
  static int dernierEtat = -1;
  
  if (etatBouton != dernierEtat) {
    dernierEtat = etatBouton;
    
    if (etatBouton == LOW) {
      digitalWrite(LED_Rouge, LOW);
      digitalWrite(LED_Verte, HIGH);
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LED Verte ON");
      lcd.setCursor(0, 1);
      lcd.print("Rouge OFF");
      
      if (!cycleBuzzerActif) {
        cycleBuzzerActif = true;
        etatBuzzer = HIGH;             
        digitalWrite(BUZZER, HIGH);
        dernierChangementBuzzer = millis();
      }
    } 
    else {
      digitalWrite(LED_Rouge, HIGH);  
      digitalWrite(LED_Verte, LOW);   
      digitalWrite(BUZZER, LOW);      
      cycleBuzzerActif = false;   
      etatBuzzer = LOW;                
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LED Rouge ON");
      lcd.setCursor(0, 1);
      lcd.print("Verte OFF");
    }
  }
  
  if (cycleBuzzerActif) {
    unsigned long tempsEcoule = millis() - dernierChangementBuzzer;
    
    if (tempsEcoule >= 200) { 
      etatBuzzer = !etatBuzzer;
      digitalWrite(BUZZER, etatBuzzer);
      dernierChangementBuzzer = millis();
    }
  }
}