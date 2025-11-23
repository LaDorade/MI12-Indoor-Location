#include <WiFi.h>

const char* ssid = "Freebox-956930";
const char* password = "nevadaziga";

void connectToWifi() {
// 2. Connexion WiFi
  Serial.println("Connecting to the network");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté au WiFi");
}