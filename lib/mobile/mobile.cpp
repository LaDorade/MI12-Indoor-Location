#ifdef MOBILE
#include <WiFi.h>
#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <map>

#include "common.h"
#include "Kalman.h"
#include "mqtt.h"
#include "led.h"

#define STATIC_SERVICE_UUID "BCDA"

struct BeaconData {
  Kalman kalmanFilter;
  float lastFilteredRSSI;
  unsigned long lastSeen;
  float rssiHistory[5];
  int historyIndex;
  int historyCount;
};

std::map<String, BeaconData> beacons;

NimBLEUUID staticUUID = NimBLEUUID(STATIC_SERVICE_UUID);
NimBLEScan* pBLEScan;

// Paramètres Kalman
float defaultQ = 0.01f;
float defaultR = 8.0f;

// Seuils de filtrage
const int RSSI_MIN                = -100;
const int RSSI_MAX                = -30;
const float RSSI_CHANGE_THRESHOLD = 15.0f;  // Seuil de changement brusque


void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println(String((char*)payload).substring(0, length));
  if (String(topic) == "capteurs/rssi/mobile") {
    String message = String((char*)payload).substring(0, length);
    if (message == "ok") {
      ledMode = LED_OK;
    } else if (message == "danger") {
      ledMode = LED_DANGER;
    }
  }
}


class MyCallback: public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        if (advertisedDevice->haveServiceUUID() && staticUUID.equals(advertisedDevice->getServiceUUID())) {
            String beaconName = advertisedDevice->getName().c_str();
            int rawRSSI = advertisedDevice->getRSSI();
            
            // Filtrer les valeurs aberrantes
            if (rawRSSI < RSSI_MIN || rawRSSI > RSSI_MAX) {
                return;
            }
            
            if (beacons.find(beaconName) == beacons.end()) {
                BeaconData newBeacon;
                newBeacon.kalmanFilter.setQ(defaultQ);
                newBeacon.kalmanFilter.setR(defaultR);
                newBeacon.lastFilteredRSSI = rawRSSI;
                newBeacon.lastSeen = 0;
                newBeacon.historyIndex = 0;
                newBeacon.historyCount = 0;
                for (int i = 0; i < 5; i++) newBeacon.rssiHistory[i] = rawRSSI;
                beacons[beaconName] = newBeacon;
                Serial.println("Nouvelle balise détectée: " + beaconName);
            }
            
            BeaconData& beacon = beacons[beaconName];
            
            // Moyenne mobile sur 5 échantillons avant Kalman
            beacon.rssiHistory[beacon.historyIndex] = rawRSSI;
            beacon.historyIndex = (beacon.historyIndex + 1) % 5;
            if (beacon.historyCount < 5) beacon.historyCount++;
            
            float avgRSSI = 0;
            for (int i = 0; i < beacon.historyCount; i++) {
                avgRSSI += beacon.rssiHistory[i];
            }
            avgRSSI /= beacon.historyCount;
            
            // Mise à jour du filtre Kalman avec la moyenne mobile
            float filteredRSSI = beacon.kalmanFilter.update(avgRSSI);
            beacon.lastFilteredRSSI = filteredRSSI;
            beacon.lastSeen = millis();
            
            // Serial.print("Balise ");
            // Serial.print(beaconName);
            // Serial.print(" - RSSI brut: ");
            // Serial.print(rawRSSI);
            // Serial.print(" | moy: ");
            // Serial.print(avgRSSI);
            // Serial.print(" | filtré: ");
            // Serial.println(filteredRSSI);

            // MQTT publication
            String topic = baseTopic + beaconName;
            char rssiString[16];
            dtostrf(filteredRSSI, 8, 2, rssiString);
            client.publish(topic.c_str(), rssiString);
        }
    }
};

void start() {
  Serial.begin(9600);
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());

  // LED setup
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(10);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green1; 
  }
  FastLED.show();


  // Connect to WiFi
  connectToWifi();
  
  // MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
  reconnect();
  Serial.println("Subscribing to topic");
  while (!client.subscribe("capteurs/rssi/mobile")) {
    delay(200);
    Serial.print(".");
  }

  //* Init bluetooth scan *//
  NimBLEDevice::init("MobileTag");
  pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setScanCallbacks(new MyCallback());
  
  pBLEScan->setDuplicateFilter(false);
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(50);
  pBLEScan->setMaxResults(0);

  pBLEScan->start(0, false);
  Serial.println("Scan BLE en cours...");
}

int prevMillis      = 0;
int currentMillis   = millis();

void callback() {
  currentMillis = millis();
  if (currentMillis - prevMillis >= 2000) {
    prevMillis = currentMillis;
    if (!client.connected()) {
      reconnect();
    }
  }
  client.loop();

  if (ledMode == LED_OK) {
    setLedOk();
  } else if (ledMode == LED_DANGER) {
    setLedDanger();
  }
}

#endif