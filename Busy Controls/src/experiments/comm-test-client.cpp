#include <Arduino.h>
#include <SPI.h>
#include <avr/io.h>

#define LED_PIN PIN_B0

// Pins (e.g. PIN_Ax) refer to an pin on the device, vs PAx which refers to a bit
// on that port (A, B, etc)
#define DO_PIN PIN_A5
#define DI_PIN PIN_A6
#define SCLK_PIN PIN_A4
#define SS_BIT PA7
#define SS_PIN PIN_A7

// Clear the overflow flag after reading data from an SPI transfer
#define CLEAR_OVERFLOW() USISR |= _BV(USIOIF)

// Test whether the overflow condition has happened
#define IN_OVERFLOW() (USISR & _BV(USIOIF))
#define NOT_IN_OVERFLOW() !IN_OVERFLOW()

// Read the value off the chip select
#define CHIP_SELECT() (PINA & _BV(SS_BIT))
#define IS_CHIP_SELECTED() (CHIP_SELECT() == 0)

#define LIGHT_ON digitalWrite(LED_PIN, HIGH)
#define LIGHT_OFF digitalWrite(LED_PIN, LOW)

void init_spi(void) {
    // USIWM0 sets 3 wire mode, USICS1 sets an external clock source
    USICR = _BV(USIWM0) | _BV(USICS1);

    // Clear the overflow flag
    CLEAR_OVERFLOW();
}

void reset_overflow() {
    CLEAR_OVERFLOW();
}

uint8_t val_in, val_out;
void setup() {
    pinMode(DO_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    pinMode(DI_PIN, INPUT);
    pinMode(PIN_PA7, INPUT);
    pinMode(SCLK_PIN, INPUT);

    init_spi();

    val_in = 1;
    val_out = 2;

    LIGHT_OFF;

    // Make sure that we clear the overflow as soon as our chip is selectced.  This makes
    // sure that we can adjust to any weirdness from the host (e.g. new code is uploaded).
    // Without this, the whole system needs to be power cycled to get back in sync.
    attachInterrupt(digitalPinToInterrupt(SS_PIN), reset_overflow, FALLING);
}

void loop() {
    // While in chip select, read data
    while (IS_CHIP_SELECTED()) {
        LIGHT_ON;

        // Wait for bytes to be read in, at which point overflow will trigger
        while (NOT_IN_OVERFLOW() && IS_CHIP_SELECTED());

        // Read the value we got in and immediately write out the value we want to send forward
        val_in = USIDR;
        USIDR = val_out;
        CLEAR_OVERFLOW();

        val_out = val_in * 2;

        LIGHT_OFF;
    }
    val_out = 0;
    USIDR = val_out;
}