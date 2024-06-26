#include <Arduino.h>
#include <FastLED.h>

#define DEMO_PIN_CLOBBER true

#define NEO_PIN 10
//#define NEO_PIN PIN_PB0
#define NUM_LEDS 12

#define CLK_PIN PIN_PA0
#define DT_PIN PIN_PA1
#define BUTTON_PIN PIN_PA2

CRGB leds[NUM_LEDS];

void clearLeds() {
    for (int i=0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
}

void cycleLeds() {
    uint8_t hue = 0;
    uint8_t sat = 255;
    uint8_t val = 255;

    for (int i=0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hue, sat, val);
        hue += (int) 255.0/NUM_LEDS;
        
        FastLED.show();
        delay(500);
    }
}

void setup() {
    if (DEMO_PIN_CLOBBER) {
        pinMode(CLK_PIN, INPUT);
        pinMode(DT_PIN, INPUT);
        pinMode(BUTTON_PIN, INPUT);

        // Set the pullup resistor on the button pin
        digitalWrite(BUTTON_PIN, HIGH);

        FastLED.addLeds<WS2812B, NEO_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
    } else {
        FastLED.addLeds<WS2812B, NEO_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
        pinMode(CLK_PIN, INPUT);
    }
}

void loop() {
    clearLeds();
    cycleLeds();
}
