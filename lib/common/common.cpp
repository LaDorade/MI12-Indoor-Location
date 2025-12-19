#ifndef ACCESS_POINT
#include <WiFi.h>

const char* ssid = "esp8266_ap";
const char* password = "testtest";

IPAddress staticIP(192, 168, 4, IP); // ESP32 static IP
IPAddress gateway(192, 168, 4, 1);    // IP Address of your network gateway (router)
IPAddress subnet(255, 255, 255, 0);   // Subnet mask
IPAddress primaryDNS(192, 168, 4, 1); // Primary DNS (optional)
IPAddress secondaryDNS(0, 0, 0, 0);   // Secondary DNS (optional)

void connectToWifi() {
// 2. Connexion WiFi
  Serial.println("Connecting to the network");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Configuring static IP
  if(!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Failed to configure Static IP");
  } else {
    Serial.println("Static IP configured!");
  }
  
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}
#endif