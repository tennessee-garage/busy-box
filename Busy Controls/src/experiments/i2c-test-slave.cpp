#include <Arduino.h>
#include <TinyWireS.h>

#define I2C_SLAVE_ADDRESS 0x3
#define LED_PIN PIN_B0

void requestEvent() {
  if (i%2==0) {
    digitalWrite(LED_PIN, LOW);
  } else
  {digitalWrite(LED_PIN, HIGH);}
  TinyWireS.send(i);
  i++;
}

void setup() {
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onRequest(requestEvent);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

void loop() {
    TinyWireS_stop_check();
}