#ifdef STATIC

#include <Arduino.h>
#include <WiFi.h>
#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

NimBLEUUID mobileUUID = NimBLEUUID("ABCD");

class MyCallback: public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        if (advertisedDevice->haveServiceUUID() && mobileUUID.equals(advertisedDevice->getServiceUUID())) {
            NimBLEUUID devUUID = advertisedDevice->getServiceUUID();
            Serial.print("Found ServiceUUID: ");
            Serial.println(devUUID.toString().c_str());
            Serial.println("RSSI: " + String(advertisedDevice->getRSSI()));
            Serial.println("");
        }
    }
};
void start()
{
  Serial.begin(9600);
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());

  // blue
  NimBLEDevice::init("");
  
  pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setScanCallbacks(new MyCallback());
  
  // On scanne agressivement
  pBLEScan->setActiveScan(true); // Demande plus d'infos (consomme plus)
  pBLEScan->setInterval(45);     // Intervalle de scan (ms)
  pBLEScan->setWindow(45);       // Fenêtre d'écoute (ms) -> 100% du temps d'écoute
  pBLEScan->setMaxResults(0);    // Pas de limite de résultats

  Serial.println("Scan en cours...");
}

void callback()
{
  NimBLEScanResults foundDevices = pBLEScan->getResults(0, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults(); // delete results scan buffer to release memory
  delay(200);
}


#endif