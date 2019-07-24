/*
  # Serge CLAUS
  # GPL V3
  # Version 2.0
  # 02/11/2018 / 24/07/2019
*/

#include <Wire.h>

#define WIRE 5      // 1-wire (T° miroir) /!\ D3 ne fonctionne pas (T° erronée)
#define CHAUF1 9   // Chauffage miroir (MOSFET) / Lunette
#define CHAUF2 10   // Chauffage chercheur 
#define CHAUF3 11   // Chauffage Lunette guide / oculaire
#define BOUT1 2    // Bouton défilement
#define BOUT2 3    // Bouton validation

// Ecran OLED
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h>

// BME-280
#include <Adafruit_Sensor.h> 
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;

// 1wire
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(WIRE);
DallasTemperature sensors(&oneWire);
DeviceAddress therMir; //sensorDeviceAddress;


// Variables globales

void setup() {
  Serial.begin(9600);
  Serial.println("Booting");

  //Wire.begin();
  bme.begin(0x76);
  pinMode(CHAUF1, OUTPUT);
  pinMode(CHAUF2, OUTPUT);
  pinMode(CHAUF3, OUTPUT);

  // 1wire
  sensors.begin();
  sensors.getAddress(therMir, 0);
  //sensors.setResolution(therMir, 9);

  analogWrite(CHAUF2,100);
  analogWrite(CHAUF3,100);
}

void loop() {
    // Mesure T° / humidité
    float h = bme.readHumidity();
    // Lecture de la température en Celcius
    float t = bme.readTemperature();
    Serial.print("T° ext: ");Serial.print(t);Serial.print(" H%: ");Serial.println(h);
    // T° miroir
    sensors.requestTemperatures();
    float tmir = sensors.getTempC(therMir);
    //float tmir = 12.50;
    Serial.print("T° miroir: ");Serial.println(tmir);
    if (tmir != -127 && tmir != 85) {
//      Serial.println("Temp: %f Hum: %f T miroir: %f", t, h, tmir);

      // Calcul du point de rosée
      float ptRosee = CalculRosee(t, h);
      //ptRosee=25.0;
//      Serial.println(" Point de rosee: %f", ptRosee);
      // T° miroir <= point de rosée +1° ?
      if (tmir <= (ptRosee + 1.5)) {
        Serial.println(" ON CHAUFFE");
        if (!digitalRead(CHAUF1)) {
          digitalWrite(CHAUF1, HIGH);
        }
      }
      else {
        if (digitalRead(CHAUF1)) {
          digitalWrite(CHAUF1, LOW);
        }
      }
    }
    else {
      Serial.println("Problème température miroir");
      if (digitalRead(CHAUF1)) {
        digitalWrite(CHAUF1, LOW);
      }
    }
}

float CalculRosee(float t, float h) {
  float ptr = (17.27 * t) / (237.7 + t) + log(h * 0.01);
  return ((237.7 * ptr) / (17.27 - ptr));
  // return t-((100.0-h)/5.0);  Autre calcul plus approximatif...
}


