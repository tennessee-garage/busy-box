#include <Arduino.h>

#include "TinySPI.h"
#include "VUDisplay.h"

// VU meter pin
#define VU_PIN PIN_B2

// LED pins
#define RED_LED_PIN PIN_A3
#define GREEN_LED_PIN PIN_A2
#define BLUE_LED_PIN PIN_A1

// SPI Pins
#define DO_PIN PIN_A5
#define DI_PIN PIN_A6
#define SCLK_PIN PIN_A4
#define SS_PIN PIN_A7

TinySPI *spi;
VUDisplay *vu;

void red_led_on() {
    digitalWrite(RED_LED_PIN, HIGH);
}

void red_led_off() {
    digitalWrite(RED_LED_PIN, LOW);
}

void blue_led_on() {
    digitalWrite(BLUE_LED_PIN, HIGH);
}

void blue_led_off() {
    digitalWrite(BLUE_LED_PIN, LOW);
}

void green_led_on() {
    digitalWrite(GREEN_LED_PIN, HIGH);
}

void green_led_off() {
    digitalWrite(GREEN_LED_PIN, LOW);
}

void all_leds_off() {
    red_led_off();
    green_led_off();
    blue_led_off();
}

void init_leds() {
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);

    all_leds_off();
}

void test_pattern() {
    for (uint8_t i=0; i < 100; i++) {
        vu->write(i);
        if (i >= 0) {
            red_led_on();
        }
        if (i >= 33) {
            green_led_on();
        }
        if (i >= 66) {
            blue_led_on();
        }

        delay(20);
    }
}

void setup() {
    spi = new TinySPI(DO_PIN, DI_PIN, SCLK_PIN, SS_PIN);
    vu = new VUDisplay(VU_PIN, 100);
    init_leds();

    test_pattern();

    all_leds_off();
    vu->ease_to_zero();

    spi->begin();
}

void loop() {
    if (spi->poll_byte()) {
        vu->write(spi->last_byte());
    }
}
