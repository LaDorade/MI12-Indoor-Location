#ifdef STATIC

#include <Arduino.h>
#include <WiFi.h>
#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"
#include <PubSubClient.h>

#include "common.h"
#include "Kalman.h"

Kalman kalmanFilter = Kalman();

NimBLEUUID mobileUUID = NimBLEUUID("ABCD");
NimBLEScan* pBLEScan;

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "192.168.1.92"; 
const String topicRSSI = "capteurs/rssi/" + String(WiFi.macAddress());

void reconnect() {
  // Boucle jusqu'à la reconnexion
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    // Tentative de connexion avec l'ID défini
    if (client.connect(ID)) {
      Serial.println("Connecté !");
      // On peut envoyer un message "Bonjour" au démarrage
      client.publish("status", "ESP32 Alpha en ligne");
    } else {
      Serial.print("Echec, rc=");
      Serial.print(client.state());
      Serial.println(" nouvelle tentative dans 5s");
      delay(5000);
    }
  }
}

class MyCallback: public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        if (advertisedDevice->haveServiceUUID() && mobileUUID.equals(advertisedDevice->getServiceUUID())) {
            // NimBLEUUID devUUID = advertisedDevice->getServiceUUID();

            // Filtrage Kalman
            float filteredRSSI = kalmanFilter.update((float)advertisedDevice->getRSSI());
            Serial.print("RSSI filtré (Kalman): ");
            Serial.println(filteredRSSI);

            // MQTT transmet des octets/texte, pas des int bruts par défaut
            char rssiString[8];
            dtostrf(filteredRSSI, 8, 2, rssiString); 

            // 3. Publier sur le topic
            client.publish(topicRSSI.c_str(), rssiString);
        }
    }
};
void start()
{
  Serial.begin(9600);
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());

  // Connect to WiFi
  connectToWifi();

  // MQTT
  client.setServer(mqtt_server, 1883);
  reconnect();

  // blue
  NimBLEDevice::init("");
  pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setScanCallbacks(new MyCallback());
  
  pBLEScan->setDuplicateFilter(false); // allow multiple
  pBLEScan->setActiveScan(true); // Demande plus d'infos (consomme plus)
  pBLEScan->setInterval(45);     // Intervalle de scan (ms)
  pBLEScan->setWindow(45);       // Fenêtre d'écoute (ms) -> 100% du temps d'écoute
  pBLEScan->setMaxResults(0);    // Pas de limite de résultats

  // Lance le scan pour une durée infinie (0), sans bloquer le code (false)
  pBLEScan->start(0, false);
  Serial.println("Scan en cours...");
}

void callback()
{
  // Vérification connexion MQTT
  if (!client.connected()) {
    reconnect();
    delay(1000);
  }
  // Maintient la connexion active (ping heartbeat)
  client.loop();

}


#endif