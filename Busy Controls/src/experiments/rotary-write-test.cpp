#include <Arduino.h>
#include "TinySPI.h"

// Test whether the overflow condition has happened
#define IN_OVERFLOW() (USISR & _BV(USIOIF))
#define NOT_IN_OVERFLOW() !IN_OVERFLOW()

// SPI pins
#define DO_PIN PIN_A5
#define DI_PIN PIN_A6
#define SCLK_PIN PIN_A4
#define SS_PIN PIN_A7

uint8_t read_byte;
TinySPI *spi;

void setup() {
    spi = new TinySPI(DO_PIN, DI_PIN, SCLK_PIN, SS_PIN);
    spi->set_response_byte(0x06);
    /*
    pinMode(DO_PIN, OUTPUT);
    
    pinMode(DI_PIN, INPUT);
    pinMode(SS_PIN, INPUT_PULLUP);
    pinMode(SCLK_PIN, INPUT);
    */
}

void loop() {
    spi->poll_byte();
    /*
    if (digitalRead(SS_PIN) == 0) {


        // Signal to the host that we're ready
        digitalWrite(DO_PIN, LOW);
        digitalWrite(DO_PIN, HIGH);
        digitalWrite(DO_PIN, LOW);

        // Enable SPI - this fucking blows away setting DO_PIN as an output for some fucking reason
        USICR = _BV(USIWM0) | _BV(USICS1);
        // Clear the overflow flag after reading data from an SPI transfer
        USISR |= _BV(USIOIF);
        // Restablish the value we're sending
        USIDR = 6; //ENCODER->current_value();

        while (NOT_IN_OVERFLOW() && (digitalRead(SS_PIN) == 0));

        read_byte = USIDR;

        USICR = 0;
    }
    */
}
