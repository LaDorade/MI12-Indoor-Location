#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "192.168.4.99";
const String baseTopic = "capteurs/rssi/";


void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    if (client.connect("mobile_tag")) {
      Serial.println("Connecté !");
      client.publish("status", "Mobile tag en ligne");
    } else {
      Serial.print("Echec, rc=");
      Serial.print(client.state());
      Serial.println(" nouvelle tentative dans 5s");
      delay(5000);
    }
  }
}