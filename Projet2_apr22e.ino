#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int LED_Rouge = 13;
const int LED_Verte = 12;
const int BOUTON = 8;
const int BUZZER = 9;

unsigned long dernierChangementBuzzer = 0;
int etatBuzzer = LOW;          
bool cycleBuzzerActif = false;  

// Essayez ces adresses une par une : 0x27, 0x3F, 0x20
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Changez en 0x3F si ça ne marche pas

void setup() {
  Serial.begin(9600);  // Pour le débogage
  Serial.println("Démarrage...");
  
  pinMode(LED_Rouge, OUTPUT);
  pinMode(LED_Verte, OUTPUT);
  pinMode(BOUTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  // Initialisation du LCD avec vérification
  lcd.init();  // Utilisez init() au lieu de begin()
  lcd.backlight();
  lcd.clear();
  
  // Test d'affichage
  lcd.setCursor(0, 0);
  lcd.print("Test LCD");
  delay(2000);  // Laissez le temps de voir le test
  
  digitalWrite(LED_Rouge, HIGH);  
  digitalWrite(LED_Verte, LOW);   
  digitalWrite(BUZZER, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LED Rouge");
  lcd.setCursor(0, 1);
  lcd.print(" LED Verte ");
  
  Serial.println("Setup terminé");
}

void loop() {
  int etatBouton = digitalRead(BOUTON);
  
  // Variable pour éviter de rafraîchir le LCD trop souvent
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
      lcd.print("Rouge OFF    ");
      
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
      lcd.print("Verte OFF    ");
    }
  }
  
  // Gestion du buzzer (indépendante du rafraîchissement LCD)
  if (cycleBuzzerActif) {
    unsigned long tempsEcoule = millis() - dernierChangementBuzzer;
    
    if (tempsEcoule >= 200) { 
      etatBuzzer = !etatBuzzer;
      digitalWrite(BUZZER, etatBuzzer);
      dernierChangementBuzzer = millis();
    }
  }
}