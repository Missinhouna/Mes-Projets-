#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int LED_Rouge = 13;
const int LED_Verte = 12;
const int BOUTON = 8;
const int BUZZER = 9;

unsigned long dernierChangementBuzzer = 0;
int etatBuzzer = LOW;          
bool cycleBuzzerActif = false;  

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(LED_Rouge, OUTPUT);
  pinMode(LED_Verte, OUTPUT);
  pinMode(BOUTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  lcd.begin(16,2);
  lcd.backlight();
  lcd.clear();

   delay(100);

  digitalWrite(LED_Rouge, HIGH);  
  digitalWrite(LED_Verte, LOW);   
  digitalWrite(BUZZER, LOW);


  lcd.setCursor(0, 0);
  lcd.print("LED Rouge");
  lcd.setCursor(0, 1);
  lcd.print(" LED Verte ");

}

void loop() {
  int etatBouton = digitalRead(BOUTON);
  

  if (etatBouton == LOW) {
  
    digitalWrite(LED_Rouge, LOW);
    digitalWrite(LED_Verte, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LED Rouge");
    lcd.setCursor(0, 1);
    lcd.print(" LED Verte ");
    
  
    if (!cycleBuzzerActif) {
      cycleBuzzerActif = true;
      etatBuzzer = HIGH;             
      digitalWrite(BUZZER, HIGH);
      dernierChangementBuzzer = millis();
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
  else {

    digitalWrite(LED_Rouge, HIGH);  
    digitalWrite(LED_Verte, LOW);   
    digitalWrite(BUZZER, LOW);      
    cycleBuzzerActif = false;   
    etatBuzzer = LOW;                

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LED Rouge");
    lcd.setCursor(0, 1);
    lcd.print("LED Verte");
  }
}