#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include <static.h>
#include <mobile.h>


void setup() {
  start();
}

void loop() {
  callback();
}
