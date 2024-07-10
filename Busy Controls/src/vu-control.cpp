#include <Arduino.h>

#include "TinySPI.h"
#include "VUDisplay.h"

// VU meter pin
#define VU_PIN PIN_B2

// LED pins
#define RED_LED_PIN PIN_A3
#define GREEN_LED_PIN PIN_A2
#define BLUE_LED_PIN PIN_A1

// LED bit positions for command to turn LEDs on/off
#define RED_CMD_BIT 0x1
#define GREEN_CMD_BIT 0x2
#define BLUE_CMD_BIT 0x4

// SPI Pins
#define DO_PIN PIN_A5
#define DI_PIN PIN_A6
#define SCLK_PIN PIN_A4
#define SS_PIN PIN_A7

TinySPI *SPI;
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

void write_leds(uint8_t val) {
    if (val & RED_CMD_BIT) {
        red_led_on();
    } else {
        red_led_off();
    }

    if (val & GREEN_CMD_BIT) {
        green_led_on();
    } else {
        green_led_off();
    }

    if (val & BLUE_CMD_BIT) {
        blue_led_on();
    } else {
        blue_led_off();
    }
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

bool is_vu_set(uint8_t val) {
    return (val & 0x80) == 0;
}

bool is_led_set(uint8_t val) {
    return (val & 0x80) == 0x80;
}

void setup() {
    SPI = new TinySPI(DO_PIN, DI_PIN, SCLK_PIN, SS_PIN);
    vu = new VUDisplay(VU_PIN, 100);
    init_leds();

    test_pattern();

    all_leds_off();
    vu->ease_to_zero();

    pinMode(PIN_PA0, OUTPUT);
    digitalWrite(PIN_PA0, LOW);
}

void loop() {
    if (SPI->poll_byte()) {
        uint8_t byte = SPI->last_byte();

        if (is_vu_set(byte)) {
            vu->write(byte);
        } else if (is_led_set(byte)) {
    digitalWrite(PIN_PA0, HIGH);
    digitalWrite(PIN_PA0, LOW);
            write_leds(byte);
        }
    }
}
