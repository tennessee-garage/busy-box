#include <Arduino.h>

#define LED_PIN PIN_B0

#define LIGHT_ON digitalWrite(LED_PIN, HIGH)
#define LIGHT_OFF digitalWrite(LED_PIN, LOW)

void setup() {
    Serial.begin(115200);
    Serial.println("Setup begin");

    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    Serial.println("Running loop ...");
    LIGHT_ON;
    delay(500);
    LIGHT_OFF;
    delay(500);
}
