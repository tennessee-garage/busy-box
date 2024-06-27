#include <Arduino.h>
#include <FastLED.h>
#include "EncoderControl.h"
#include "RingLED.h"

#define CLK_PIN PIN_PA0
#define DT_PIN PIN_PA1
#define BUTTON_PIN PIN_PA2

#define NUM_LEDS 12
#define NUM_PATTERNS 3

EncoderControl *ENCODER;
RingLED *RING;

static uint8_t pattern = 0;

void changeRingDisplay(uint8_t pattern) {
    switch(pattern) {
        case 0:
            RING->set_display_method(RedDotUpdate);
            break;
        case 1:
            RING->set_display_method(GreenDotUpdate);
            break;
        default:
            RING->set_display_method(DoubleIndicator);
            break;
    }
}

void setup() {
    RING = new RingLED(NUM_LEDS);
    RING->test_pattern();

    ENCODER = new EncoderControl(DT_PIN, CLK_PIN, BUTTON_PIN, NUM_LEDS - 1);
}

void loop() {
    ENCODER->poll();

    if (ENCODER->was_button_pressed()) {
        pattern = (pattern+1)%NUM_PATTERNS;
        changeRingDisplay(pattern);
        ENCODER->clear_button_press();
    }

    RING->display(ENCODER->current_value());
}
