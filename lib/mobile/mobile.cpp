#ifdef MOBILE
#include <WiFi.h>
#include <NimBLEDevice.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Define screen width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define TAG_NAME "ESP32_TAG_01"
#define SERVICE_UUID "ABCD"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The -1 argument means we are sharing the ESP32 reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void start() {
  Serial.begin(9600);
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());

  //* Initialize the OLED display *//
  // Initialize with the I2C addr 0x3C (common for 128x64)
  // If this fails, try 0x3D
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer (remove the Adafruit splash screen)
  display.clearDisplay();

  // Set Text settings
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner

  // Display static text
  display.println("Hello, World!");
  display.println("ESP32 is working.");

  display.display();


  //* Init bluetooth *//
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