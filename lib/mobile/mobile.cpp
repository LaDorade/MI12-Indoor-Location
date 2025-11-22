#ifdef MOBILE
#include <WiFi.h>
#include <NimBLEDevice.h>

#define TAG_NAME "ESP32_TAG_01"
#define SERVICE_UUID "ABCD"

void start() {
  Serial.begin(9600);
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());

  NimBLEDevice::init("NimBLE");
  
  NimBLEServer *pServer = NimBLEDevice::createServer();
  NimBLEService *pService = pServer->createService("ABCD");
  NimBLECharacteristic *pCharacteristic = pService->createCharacteristic("1234");
  
  pService->start();
  pCharacteristic->setValue("Hello BLE");
  
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID); // advertise the UUID of our service
  pAdvertising->setName(TAG_NAME); // advertise the device name
  pAdvertising->start(); 
}

void callback() {
  delay(200);
}

#endif