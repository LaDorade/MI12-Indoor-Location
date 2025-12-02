#ifdef ACCESS_POINT
#include <ESP8266WiFi.h>

// const char* wifi_network_ssid     = "Freebox-956930";
// const char* wifi_network_password = "nevadaziga";

const char* ap_ssid     = "esp8266_ap";
const char* ap_password = "testtest";

IPAddress local_ip(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

void start()
{
    Serial.begin(9600);
    Serial.println("\n[*] Creating AP");

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ap_ssid, ap_password);
    
    Serial.print("[+] AP Created with IP Gateway ");
    Serial.println(WiFi.softAPIP());
}


void callback()
{
}
#endif