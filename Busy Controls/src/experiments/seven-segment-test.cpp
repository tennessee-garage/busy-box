#include <Arduino.h>
#include <SegmentedDisplay.h>

#define SERIAL_IN_PIN PIN_A0
#define SERIAL_CLOCK_PIN PIN_A1
#define SERIAL_CLEAR_PIN PIN_A2
#define REGISTER_CLOCK_PIN PIN_A3
#define OUTPUT_ENABLE_PIN PIN_B0

#define LED_PIN PIN_B1

#define NUM_DISPLAYS 2

SegmentedDisplay *DISP;

void test_segments() {
    for (int x = 0; x < pow(10, NUM_DISPLAYS); x++) {
        DISP->write(x);
        delay(200);
    }
}

void led_on() {
    digitalWrite(LED_PIN, HIGH);
}

void led_off() {
    digitalWrite(LED_PIN, LOW);
}

void setup() {
    DISP = new SegmentedDisplay(SERIAL_IN_PIN,
                                SERIAL_CLOCK_PIN, 
                                SERIAL_CLEAR_PIN, 
                                REGISTER_CLOCK_PIN, 
                                OUTPUT_ENABLE_PIN, 
                                NUM_DISPLAYS);
}

void loop() {
    led_on();
    test_segments();
    led_off();
    delay(500);
}
