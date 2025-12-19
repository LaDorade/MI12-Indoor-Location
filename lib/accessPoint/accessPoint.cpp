#ifdef ACCESS_POINT
#include <ESP8266WiFi.h>

// const char* wifi_network_ssid     = "Freebox-956930";
// const char* wifi_network_password = "nevadaziga";

const char* ap_ssid     = "esp8266_ap";
const char* ap_password = "testtest";

IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

void start()
{
    Serial.begin(9600);
    Serial.println("\n[*] Creating AP");

    WiFi.mode(WIFI_AP);
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
    WiFi.softAP(ap_ssid, ap_password);
    
    Serial.print("[+] AP Created with IP Gateway ");
    Serial.println(WiFi.softAPIP());
}


void callback()
{
}
#endif