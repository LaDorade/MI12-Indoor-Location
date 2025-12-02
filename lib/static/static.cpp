#ifdef STATIC

#include <Arduino.h>
#include <WiFi.h>
#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

#include "common.h"
#include "Kalman.h"
#include "html.h"

Kalman kalmanFilter = Kalman();
Preferences preferences;

float rssiAt1m = -59.0f; // default RSSI at 1 meter
bool calibrationMode = false;
float calibrationSum = 0.0f;
int calibrationCount = 0;
unsigned long calibrationStartTime = 0;
const unsigned long CALIBRATION_DURATION_IN_MS = 10000;

NimBLEUUID mobileUUID = NimBLEUUID("ABCD");
NimBLEScan* pBLEScan;

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer server(80);

IPAddress local_IP(192, 168, 1, IP);
IPAddress gateway(192, 168, 1, 1);

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


String processor(const String& var) {
  if (var == "ID") return String(ID);
  if (var == "IP") return WiFi.localIP().toString();
  if (var == "MAC") return WiFi.macAddress();
  if (var == "RSSI1M") return String(rssiAt1m, 1);
  if (var == "Q") return String(kalmanFilter.getQ(), 2);
  if (var == "R") return String(kalmanFilter.getR(), 2);
  return String();
}

void loadSettings() {
  preferences.begin("static", true);
  rssiAt1m = preferences.getFloat("rssi1m", -59.0f);
  float q = preferences.getFloat("kalmanQ", 0.1f);
  float r = preferences.getFloat("kalmanR", 2.0f);
  preferences.end();
  
  kalmanFilter.setQ(q);
  kalmanFilter.setR(r);
  
  Serial.println("Paramètres chargés - RSSI@1m: " + String(rssiAt1m) + " Q: " + String(q) + " R: " + String(r));
}

void saveSettings() {
  preferences.begin("static", false);
  preferences.putFloat("rssi1m", rssiAt1m);
  preferences.putFloat("kalmanQ", kalmanFilter.getQ());
  preferences.putFloat("kalmanR", kalmanFilter.getR());
  preferences.end();
}

void setupWebServer() {
  // Page principale
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  
  // Démarrer la calibration
  server.on("/calibrate", HTTP_GET, [](AsyncWebServerRequest *request) {
    calibrationMode = true;
    calibrationSum = 0.0f;
    calibrationCount = 0;
    calibrationStartTime = millis();
    request->send(200, "text/plain", "Calibration started");
  });
  
  // Obtenir le résultat de la calibration
  server.on("/calibration_result", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{\"rssi\":" + String(rssiAt1m, 1) + ",\"count\":" + String(calibrationCount) + "}";
    request->send(200, "application/json", json);
  });
  
  // Définir le RSSI manuellement
  server.on("/set_rssi", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      rssiAt1m = request->getParam("value")->value().toFloat();
      saveSettings();
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing value");
    }
  });
  
  // Configurer les paramètres Kalman
  server.on("/set_kalman", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("q") && request->hasParam("r")) {
      float q = request->getParam("q")->value().toFloat();
      float r = request->getParam("r")->value().toFloat();
      kalmanFilter.setQ(q);
      kalmanFilter.setR(r);
      saveSettings();
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });
  
  // Réinitialiser le filtre Kalman
  server.on("/reset_kalman", HTTP_GET, [](AsyncWebServerRequest *request) {
    kalmanFilter.reset();
    request->send(200, "text/plain", "OK");
  });
  
  // API pour obtenir les valeurs actuelles en JSON
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"id\":\"" + String(ID) + "\",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"rssi1m\":" + String(rssiAt1m, 1) + ",";
    json += "\"kalmanQ\":" + String(kalmanFilter.getQ(), 3) + ",";
    json += "\"kalmanR\":" + String(kalmanFilter.getR(), 3) + ",";
    json += "\"currentEstimate\":" + String(kalmanFilter.getEstimate(), 2);
    json += "}";
    request->send(200, "application/json", json);
  });
  
  server.begin();
  Serial.println("Serveur web démarré sur http://" + WiFi.localIP().toString());
}

class MyCallback: public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        if (advertisedDevice->haveServiceUUID() && mobileUUID.equals(advertisedDevice->getServiceUUID())) {
            int rawRSSI = advertisedDevice->getRSSI();
            
            // Mode calibration: collecter les mesures
            if (calibrationMode) {
                calibrationSum += rawRSSI;
                calibrationCount++;
                
                // Vérifier si la calibration est terminée
                if (millis() - calibrationStartTime >= CALIBRATION_DURATION_IN_MS) {
                    if (calibrationCount > 0) {
                        rssiAt1m = calibrationSum / calibrationCount;
                        saveSettings();
                        Serial.println("Calibration terminée! RSSI@1m = " + String(rssiAt1m));
                    }
                    calibrationMode = false;
                }
                return;
            }

            // Filtrage Kalman
            float filteredRSSI = kalmanFilter.update((float)rawRSSI);
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
  
  // Charger les paramètres sauvegardés
  loadSettings();
  
  // Démarrer le serveur web
  setupWebServer();

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