#ifdef STATIC

#include <Arduino.h>
#include <WiFi.h>
#include <NimBLEDevice.h>

#define SERVICE_UUID "BCDA"

void start()
{
  Serial.begin(9600);
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Beacon ID: ");
  Serial.println(ID);

  NimBLEDevice::init(ID);
  
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  
  NimBLEServer *pServer = NimBLEDevice::createServer();
  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  NimBLECharacteristic *pCharacteristic = pService->createCharacteristic("1234");
  
  pService->start();
  pCharacteristic->setValue(ID);
  
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setName(ID);
  
  pAdvertising->setMinInterval(160);  // 100ms min
  pAdvertising->setMaxInterval(200);  // 125ms max
  
  pAdvertising->start();
  
  Serial.println("Balise statique en mode émission BLE");
}

void callback()
{
  delay(200);
}

#endif