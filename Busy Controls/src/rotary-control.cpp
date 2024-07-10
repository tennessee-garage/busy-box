#include <Arduino.h>
#include <FastLED.h>
#include "EncoderControl.h"
#include "RingLED.h"
#include "TinySPI.h"

// Encoder pins
#define CLK_PIN PIN_PA0
#define DT_PIN PIN_PA1
#define BUTTON_PIN PIN_PA2

// LED ring constants
#define NUM_LEDS 12
#define NUM_PATTERNS 3

// SPI pins
#define DO_PIN PIN_A5
#define DI_PIN PIN_A6
#define SCLK_PIN PIN_A4
#define SS_PIN PIN_A7

EncoderControl *ENCODER;
RingLED *RING;
TinySPI *SPI;

// This value contains the button press state in bit 0x80 and the rotary value
// in bits 0x7F, i.e. a value between 0 and 127.
static uint8_t encoder_state;

// Keep track of the ring display method used
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

void set_encoder_state_value(uint8_t value) {
    // Set the encoder value without changing current button press bit
    encoder_state = (encoder_state & 0x80) | value;
}

void set_encoder_state_button_press() {
    encoder_state |= 0x80;
}

void clear_encoder_state_button_press() {
    encoder_state &= ~0x80;
}

void setup() {
    RING = new RingLED(NUM_LEDS);
    RING->test_pattern();
    RING->clear();
    RING->display(0);

    ENCODER = new EncoderControl(DT_PIN, CLK_PIN, BUTTON_PIN, NUM_LEDS - 1);

    SPI = new TinySPI(DO_PIN, DI_PIN, SCLK_PIN, SS_PIN);
}

void loop() {
    if (SPI->poll_byte()) {
        // If a byte was just read, clear the button press bit, whatever it was.  We only
        // want to send this out once to the host if it was set.
        clear_encoder_state_button_press();
    }

    if (ENCODER->poll()) {
        // Set the current value if it changes
        set_encoder_state_value(ENCODER->current_value());
        RING->display(ENCODER->current_value());
    }

    if (ENCODER->was_button_pressed()) {
        pattern = (pattern+1)%NUM_PATTERNS;
        changeRingDisplay(pattern);
        ENCODER->clear_button_press();

        // Toggle the button press bit in our response
        set_encoder_state_button_press();
    }

    SPI->set_response_byte(encoder_state);
}
