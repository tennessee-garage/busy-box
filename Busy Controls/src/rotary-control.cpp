#include <Arduino.h>
#include <FastLED.h>
#include "EncoderControl.h"

// This has to be 10 to control PIN_PB0 and I have no idea why :/
#define NEO_PIN     10
//#define NEO_PIN     PIN_PB0
#define NUM_LEDS    12
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define CLK_PIN PIN_PA0
#define DT_PIN PIN_PA1
#define BUTTON_PIN PIN_PA2

#define NUM_PATTERNS 3

CRGB leds[NUM_LEDS];

EncoderControl *ENCODER;

static uint8_t pattern = 0;

void colorRing() {
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

void clearLEDs() {
    for (int i=0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
}

void redDotUpdate(uint8_t value) {
    for (int i=0; i < NUM_LEDS; i++) {
        if (i == value%NUM_LEDS) {
            leds[i] = CRGB::Red;
        } else {
            leds[i] = CRGB::Blue;
        }
    }
}

void greenDotUpdate(uint8_t value) {
    for (int i=0; i < NUM_LEDS; i++) {
        if (i == value%NUM_LEDS) {
            leds[i] = CRGB::Green;
        } else {
            leds[i] = CRGB::Black;
        }
    }
}

void doubleIndicator(uint8_t value) {
    uint8_t hue = 170;
    uint8_t sat = 255;
    uint8_t val_bkg = 50;
    uint8_t val_ind = 150;
  
    for (int i=0; i < NUM_LEDS; i++) {
        if (i == value%NUM_LEDS) {
            leds[i] = CHSV(hue, sat, val_ind);
        } else {
            leds[i] = CHSV(hue, sat, val_bkg);
        }
    }
    for (int i=0; i < NUM_LEDS; i++) {
        if (i == (value/(NUM_LEDS+1))%NUM_LEDS) {
            leds[i] = CRGB::Green;
        }
    }
}

void fadeInUpdate(uint8_t value) {
    uint8_t hue = 66;
    uint8_t sat = 255;
    
    for (int i=0; i < NUM_LEDS; i++) {
        if (i <= value%NUM_LEDS) {
            leds[i] = CHSV(hue, sat, (int) 255.0*(i/NUM_LEDS)); //CRGB::Red;
        } else {
            leds[i] = CRGB::Black;
        }
    }
}

void stackingUpdate(uint8_t value) {
    uint8_t hue = 66;
    uint8_t sat = 255;
    
    for (int i=0; i < NUM_LEDS; i++) {
        if (i <= value%NUM_LEDS) {
            leds[i] = CHSV(hue, sat, (int) 255.0*(i/NUM_LEDS)); //CRGB::Red;
        } else {
            leds[i] = CRGB::Black;
        }
    }
}

void stackingFadeInUpdate(uint8_t value) {
    
}

void updateLEDs(uint8_t value) {
    if (pattern == 0) {
        redDotUpdate(value);
    } else if (pattern == 1) {
        greenDotUpdate(value);
    } else {
        doubleIndicator(value);
    }
    FastLED.show();
    /*
    if (pattern == 0) {
        redDotUpdate(value);
    } else if (pattern == 1) {
        fadeInUpdate(value);
    } else if (pattern == 2) {
        stackingUpdate(value);
    } else if (pattern == 3) {
        stackingFadeInUpdate(value);
    }
    */
}

unsigned long last_button_change = 0;
bool button_down = false;

void setup() {
    FastLED.addLeds<LED_TYPE, NEO_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
    FastLED.setBrightness(BRIGHTNESS);

    clearLEDs();
    colorRing();
    updateLEDs(0);

    ENCODER = new EncoderControl(DT_PIN, CLK_PIN, BUTTON_PIN, NUM_LEDS - 1);
}

void loop() {
    ENCODER->poll();

    if (ENCODER->was_button_pressed()) {
        pattern = (pattern+1)%NUM_PATTERNS;
        ENCODER->clear_button_press();
    }

    updateLEDs(ENCODER->current_value());
}
