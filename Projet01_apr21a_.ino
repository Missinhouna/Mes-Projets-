const int LED_DEFAUT = 13;
const int LED_BOUTON = 12;
const int BOUTON = 8;
const int BUZZER = 9;

unsigned long dernierChangementBuzzer = 0;
int etatBuzzer = LOW;           
bool cycleBuzzerActif = false;  

void setup() {
  pinMode(LED_DEFAUT, OUTPUT);
  pinMode(LED_BOUTON, OUTPUT);
  pinMode(BOUTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  

  digitalWrite(LED_DEFAUT, HIGH);   
  digitalWrite(LED_BOUTON, LOW);     
  digitalWrite(BUZZER, LOW);
}

void loop() {
  int etatBouton = digitalRead(BOUTON);
  
  if (etatBouton == LOW) {

    digitalWrite(LED_DEFAUT, LOW);
    digitalWrite(LED_BOUTON, HIGH);
    
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
    digitalWrite(LED_DEFAUT, HIGH);   
    digitalWrite(LED_BOUTON, LOW);    
    digitalWrite(BUZZER, LOW);        
    cycleBuzzerActif = false;         
    etatBuzzer = LOW;                 
  }
}