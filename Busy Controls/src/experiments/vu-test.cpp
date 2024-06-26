#include <Arduino.h>
#include "VUDisplay.h"

#define VU_PIN PIN_B2
#define LED_PIN PIN_B1

VUDisplay *vu_10;
VUDisplay *vu_full;

void led_on() {
    digitalWrite(LED_PIN, HIGH);
}

void led_off() {
    digitalWrite(LED_PIN, LOW);
}

void setup() {
    vu_10 = new VUDisplay(VU_PIN, 10);
    vu_full = new VUDisplay(VU_PIN, 255);

    pinMode(LED_PIN, OUTPUT);
}

void discrete_sweep() {
    for (int i = 0; i <= 10; i++) {
        led_on();
        vu_10->write(i);
        delay(500);
        led_off();
        delay(500);
    }
}

void continuous_sweep() {
    for (int i = 0; i <= 255; i++) {
        vu_full->write(i);
        led_on();
        delay(20);
        led_off();
    }
}

void loop() {
    discrete_sweep();
}
