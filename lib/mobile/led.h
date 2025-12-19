// LED control library
#include <FastLED.h>
#define NUM_LEDS    6
#define DATA_PIN    2
#define LED_TYPE    WS2812B 
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

enum LedMode {
  LED_OK,
  LED_DANGER
};

LedMode ledMode = LED_OK; // Default mode

void setLedOk() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green1;
  }
  FastLED.show();
}

void setLedDanger() {
  static unsigned long lastAnimTime = 0;
  static int animStep = 0;
  
  unsigned long currentTime = millis();
  
  if (currentTime - lastAnimTime >= 50) {
    lastAnimTime = currentTime;
    animStep = (animStep + 1) % 40; // Cycle de 40 étapes
    for (int i = 0; i < NUM_LEDS; i++) {
      int distance = (i - animStep) % NUM_LEDS;
      if (distance < 0) distance += NUM_LEDS;
      
      int baseIntensity = 100 + 70 * sin(animStep * 3.14159 / 20.0);
      
      int waveIntensity = 0;
      if (distance <= 3) {
        waveIntensity = 255 - (distance * 60);
      }
      
      int finalBrightness = baseIntensity + waveIntensity;
      if (finalBrightness > 255) finalBrightness = 255;
      
      leds[i] = CRGB(finalBrightness, 0, 0); // Rouge
    }
    FastLED.show();
  }
}
