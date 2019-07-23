/*
  # Serge CLAUS
  # GPL V3
  # Version 1.0
  # 02/11/2018 / 24/06/2019
*/

#include <Wire.h>

#define DHTPIN 12    // Pin sur lequel est branché le DHT
#define WIRE 2      // 1-wire (T° miroir) /!\ D3 ne fonctionne pas (T° erronée)
#define CHAUF 11     // Chauffage (MOSFET)
#define LED 9       // LED indicateur de chauffage (Pour l'instant pas utilisé.)

// DHT22
#include "DHT.h"          // Librairie des capteurs DHT
#define DHTTYPE DHT22         // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

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
  pinMode(CHAUF, OUTPUT);
  pinMode(PARK, OUTPUT);
  digitalWrite(PARK, LOW);

  // 1wire
  sensors.begin();
  sensors.getAddress(therMir, 0);
  //sensors.setResolution(therMir, 9);
}

// Timers
unsigned long prevTemp = 0;

#define intervTemp 10000

void loop() {
  // Timers
  unsigned long curMillis = millis();

  if (curMillis - prevTemp >= intervTemp) {
    // Toutes les 10 secondes
    // Mesure T° / humidité
    float h = dht.readHumidity();
    // Lecture de la température en Celcius
    float t = dht.readTemperature();
    Serial.print("T° ext: ");Serial.print(t);Serial.print(" H%: ");Serial.println(h);
//    if (!isnan(h) && !isnan(t)) {
      
//    }
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
        if (!digitalRead(CHAUF)) {
          digitalWrite(CHAUF, HIGH);
        }
      }
      else {
        if (digitalRead(CHAUF)) {
          digitalWrite(CHAUF, LOW);
        }
      }
    }
    else {
      Serial.println("Problème température miroir");
      if (digitalRead(CHAUF)) {
        digitalWrite(CHAUF, LOW);
      }
    }
    prevTemp = curMillis;

  }

}

float CalculRosee(float t, float h) {
  float ptr = (17.27 * t) / (237.7 + t) + log(h * 0.01);
  return ((237.7 * ptr) / (17.27 - ptr));
  // return t-((100.0-h)/5.0);  Autre calcul plus approximatif...
}


