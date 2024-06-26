#include <FastLED.h>
#include <avr/interrupt.h>

#define LED_PIN     A5
#define NUM_LEDS    9
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define DT_PIN 8
#define CLK_PIN 1

CRGB leds[NUM_LEDS];
int value = 0;
long state = 0;
long state_at_change = 0;
bool pin_change = false;

void change_func() {
  // pulses are ~37 ms apart slow turn, ~5 ms fast turn
  pin_change = true;
}

void setup() {
  pinMode(DT_PIN, INPUT);
  pinMode(CLK_PIN, INPUT);

  // put your setup code here, to run once:
  attachInterrupt(0, change_func, CHANGE);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
}

void updateLEDs() {
  for (int i=0; i < NUM_LEDS; i++) {
    if (i == value%9) {
      leds[i] = CRGB::Red;
    } else {
      leds[i] = CRGB::Blue;
    }
  }
  FastLED.show();
}

void revealState() {
  for (int i=0; i < NUM_LEDS-1; i++) {
    if ((state_at_change >> i) & 0x0001) {
      leds[i] = CRGB::Red;
    } else {
      leds[i] = CRGB::Blue;
    }
  }
  leds[8] = CRGB::Green;
  FastLED.show();
}

void loop() {
  state=(state<<1) | digitalRead(DT_PIN) | 0xe0;
  
  if (pin_change) {
    value++;
    state_at_change = state;
    /*
    // CLK pin has been zero for 12 cycles and just turned to 1
    // or 1 for 12 cycles and just turned to 0
    if ((state == 0xF0) || (state == 0xEF)) {
      value++;
      
      // If the pins are the same, clockwise, otherwise counter-clockwise
      if (digitalRead(DT_PIN) == digitalRead(CLK_PIN)) {
        value++;
      } else {
        value--;
      }
    }
    */
   
    pin_change = false;
  }
  //updateLEDs();
  revealState();
}
