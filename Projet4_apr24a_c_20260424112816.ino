/*
  Projet IoT – Monitoring Environnemental
  Capteurs : DHT22 (température/humidité) + MQ-135 (CO2/qualité air)
  Envoi vers ThingSpeak (channel 3356600)
  Option : écran LCD I2C 16x2 + relais d'alarme
*/

#include <WiFi.h>
#include "DHT.h"
#include <ThingSpeak.h>
#include <Wire.h>         // Pour I2C (LCD)
#include <LiquidCrystal_I2C.h>   // Bibliothèque pour LCD I2C

// ==================== PARAMÈTRES À MODIFIER ====================
const char* ssid = "ALOPRO_NETWORK-2G";
const char* password = "Alopro512wifi";

// ThingSpeak
unsigned long myChannelID = 3356600;
const char* myWriteAPIKey = "YTRTROPLOXC27IRS";   // Write API Key

// Brochages capteurs
#define DHTPIN 4          // DHT22 sur GPIO4
#define DHTTYPE DHT22
#define MQ135_PIN 34      // Sortie analogique du MQ-135 sur GPIO34

// Option : relais (alarme si CO2 > seuil)
#define RELAIS_PIN 13     // Relais sur GPIO13
#define SEUIL_CO2 1000    // ppm (valeur analogique brute, à calibrer)

// Option : écran LCD I2C (adresse typique 0x27 ou 0x3F)
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// ==================== INSTANCIATIONS ====================
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Variables
float temperature = 0;
float humidite = 0;
int valeurCO2 = 0;        // valeur brute du MQ-135 (0-4095)
unsigned long dernierEnvoi = 0;
const long intervalleEnvoi = 20000; // 20 secondes minimum ThingSpeak

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(MQ135_PIN, INPUT);
  pinMode(RELAIS_PIN, OUTPUT);
  digitalWrite(RELAIS_PIN, LOW);  // Relais éteint au départ

  // Initialisation LCD (optionnel)
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initialisation");
  lcd.setCursor(0, 1);
  lcd.print("WiFi...");

  // Connexion WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connecté !");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi OK");
  delay(2000);
  lcd.clear();

  ThingSpeak.begin(client);
}

// ==================== LOOP ====================
void loop() {
  // Lire les capteurs toutes les 2 secondes, mais n'envoyer que toutes les 20s
  lireCapteurs();
  afficherLCD();

  // Gestion de l'alarme (si CO2 dépasse seuil)
  if (valeurCO2 > SEUIL_CO2) {
    digitalWrite(RELAIS_PIN, HIGH);
    Serial.println("⚠️ Alarme CO2 déclenchée !");
    lcd.setCursor(0, 1);
    lcd.print("ALARME CO2  ");
  } else {
    digitalWrite(RELAIS_PIN, LOW);
  }

  // Envoi périodique vers ThingSpeak
  if (millis() - dernierEnvoi >= intervalleEnvoi) {
    envoyerThingSpeak();
    dernierEnvoi = millis();
  }

  delay(2000); // on relit les capteurs toutes les 2 secondes
}

// ---------- Lecture DHT22 et MQ-135 ----------
void lireCapteurs() {
  // Lecture DHT
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t)) temperature = t;
  if (!isnan(h)) humidite = h;

  // Lecture MQ-135 (valeur analogique 0-4095)
  valeurCO2 = analogRead(MQ135_PIN);

  // Affichage série
  Serial.print("Temp: "); Serial.print(temperature); Serial.print(" °C | ");
  Serial.print("Hum: "); Serial.print(humidite); Serial.println(" %");
  Serial.print("CO2 (brut): "); Serial.println(valeurCO2);
}

// ---------- Affichage sur LCD I2C ----------
void afficherLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C H:");
  lcd.print(humidite, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("CO2:");
  lcd.print(valeurCO2);
  lcd.print("   ");  // efface anciens caractères
}

// ---------- Envoi des données vers ThingSpeak ----------
void envoyerThingSpeak() {
  // Mettre les valeurs dans les champs
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidite);
  ThingSpeak.setField(3, valeurCO2);

  int httpCode = ThingSpeak.writeFields(myChannelID, myWriteAPIKey);

  if (httpCode == 200) {
    Serial.println("✅ Données envoyées à ThingSpeak");
  } else {
    Serial.print("❌ Erreur ThingSpeak, code : ");
    Serial.println(httpCode);
  }
}