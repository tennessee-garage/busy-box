#include <Arduino.h>
#include <avr/io.h>
#include "VUDisplay.h"

#define VU_PIN PIN_B2

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

VUDisplay *vu;
enum commands {
    WRITE=0xA1
};

void init_spi() {
    // USIWM0 sets 3 wire mode, USICS1 sets an external clock source
    USICR = _BV(USIWM0) | _BV(USICS1);

    // Clear the overflow flag
    CLEAR_OVERFLOW();
}

void reset_overflow() {
    CLEAR_OVERFLOW();
}

uint8_t val_in;
uint8_t val_cmd[2] = {0, 0};


bool receive_byte() {
    bool data_received = false;

   // While in chip select, read data
    while (IS_CHIP_SELECTED()) {
        // Wait for bytes to be read in, at which point overflow will trigger
        while (NOT_IN_OVERFLOW() && IS_CHIP_SELECTED());

        // Read the value we got in and immediately write out the value we want to send forward
        val_in = USIDR;
        USIDR = val_in + 1;
        CLEAR_OVERFLOW();
        data_received = true;
    }

    return data_received;
}

bool receive_command() {
    uint8_t byte_num = 0;
    bool data_received = false;

    // While in chip select, read data
    while (IS_CHIP_SELECTED()) {
        // Wait for bytes to be read in, at which point overflow will trigger
        while (NOT_IN_OVERFLOW() && IS_CHIP_SELECTED());

        // Read the value we got in and immediately write out the value we want to send forward
        val_cmd[byte_num] = USIDR;
        USIDR = val_cmd[byte_num] + 1;
        CLEAR_OVERFLOW();
        data_received = true;

        byte_num++;
    }

    if (!data_received) {
        return false;
    }

    if (byte_num !=2) {
        return false;
    }

    return true;
}

void setup() {
    vu = new VUDisplay(VU_PIN, 100);

    pinMode(DO_PIN, OUTPUT);

    pinMode(DI_PIN, INPUT);
    pinMode(PIN_PA7, INPUT);
    pinMode(SCLK_PIN, INPUT);

    init_spi();

    // Make sure that we clear the overflow as soon as our chip is selectced.  This makes
    // sure that we can adjust to any weirdness from the host (e.g. new code is uploaded).
    // With out this, the whole system needs to be power cycled to get back in sync.
    attachInterrupt(digitalPinToInterrupt(SS_PIN), reset_overflow, FALLING);
}

void loop() {
    if (receive_byte()) {
        vu->write(val_in);
    }
}
