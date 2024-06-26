#include <FastLED.h>

#define NEO_PIN     10
#define NUM_LEDS    12
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define DT_PIN A0
#define CLK_PIN A1

#define BUTTON_PIN 2
#define BUTTON_DEBOUNCE_MS 5
#define NUM_PATTERNS 3

CRGB leds[NUM_LEDS];

static uint8_t prevNextCode = 0;
static uint16_t store=0;
static uint8_t pattern = 0;

void setup() {
  pinMode(DT_PIN, INPUT);
  pinMode(CLK_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(NEO_PIN, OUTPUT);

  // Set the pullup resistor on the button pin
  digitalWrite(BUTTON_PIN, HIGH);
  
  FastLED.addLeds<LED_TYPE, NEO_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);

  clearLEDs();
  colorRing();
  updateLEDs(0);
}

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

int8_t read_rotary() {
  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

  prevNextCode <<= 2;
  if (digitalRead(DT_PIN)) prevNextCode |= 0x02;
  if (digitalRead(CLK_PIN)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[prevNextCode] ) {
      store <<= 4;
      store |= prevNextCode;
      if ((store&0xff)==0x2b) return -1;
      if ((store&0xff)==0x17) return 1;
   }
   return 0;
}

void clearLEDs() {
  for (int i=0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
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

unsigned long last_button_change = 0;
bool button_down = false;

void loop() {
  static int8_t c, val;

  if (!button_down &&
      (digitalRead(BUTTON_PIN) == 0) &&
      ((millis() - last_button_change) > BUTTON_DEBOUNCE_MS)) {
    last_button_change = millis();
    button_down = true;

    pattern = (pattern+1)%NUM_PATTERNS;
    updateLEDs(c);
  }

  if (digitalRead(BUTTON_PIN)) {
    button_down = false;
    last_button_change = millis();
  }
  
  if (val=read_rotary()) {
    c +=val;

    updateLEDs(c);
  }
}
