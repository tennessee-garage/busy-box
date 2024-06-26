#include "TinySPI.h"

// Clear the overflow flag after reading data from an SPI transfer
#define CLEAR_OVERFLOW() USISR |= _BV(USIOIF)

// Test whether the overflow condition has happened
#define IN_OVERFLOW() (USISR & _BV(USIOIF))
#define NOT_IN_OVERFLOW() !IN_OVERFLOW()

TinySPI* TinySPI::instance;

TinySPI::TinySPI(uint8_t do_pin, uint8_t di_pin, uint8_t clk_pin, uint8_t en_pin) {
    _do_pin = do_pin;
    _di_pin = di_pin;
    _clk_pin = clk_pin;
    enable_pin = en_pin;

    pinMode(_do_pin, OUTPUT);
    pinMode(_di_pin, INPUT);
    pinMode(enable_pin, INPUT);
    pinMode(_clk_pin, INPUT);

    // Configure the enable pin to have a pullup resistor by writing high to it
    digitalWrite(enable_pin, HIGH);

    // USIWM0 sets 3 wire mode, USICS1 sets an external clock source
    USICR = _BV(USIWM0) | _BV(USICS1);
}

void TinySPI::begin() {
    // Enable pin change interrupt on tne enable pin
    GIMSK |= (1 << PCIE0);

    // Enable appropriate interrupt for the given select pin 
    switch (enable_pin) {
        case PIN_A0:
            PCMSK0 |= (1 << PCINT0);
            break;
        case PIN_A1:
            PCMSK0 |= (1 << PCINT1);
            break;
        case PIN_A2:
            PCMSK0 |= (1 << PCINT2);
            break;
        case PIN_A3:
            PCMSK0 |= (1 << PCINT3);
            break;
        case PIN_A4:
            PCMSK0 |= (1 << PCINT4);
            break;
        case PIN_A5:
            PCMSK0 |= (1 << PCINT5);
            break;
        case PIN_A6:
            PCMSK0 |= (1 << PCINT6);
            break;
        case PIN_A7:
            PCMSK0 |= (1 << PCINT7);
            break;
        default:
            break;
    }

    instance = this;

    // Enable global interrupts
    sei();
}

ISR(PCINT0_vect) {
    if (TinySPI::instance == nullptr) {
        return;
    }

    if (digitalRead(TinySPI::instance->enable_pin) == 0) {
        // Clear overflow when we see the falling edge of the enable pin
        CLEAR_OVERFLOW();
    }
}

bool TinySPI::_is_chip_selected() {
    return digitalRead(enable_pin) == 0;
}

bool TinySPI::poll_byte() {
    if (!_is_chip_selected()) {
        return false;
    }

    // Wait for bytes to be read in, at which point overflow will trigger
    while (NOT_IN_OVERFLOW());

    // Read the value we got in and immediately write out the value we want to send forward
    _last_byte = USIDR;
    USIDR = _last_byte + 1;

    return true;
}

uint8_t TinySPI::last_byte() {
    return _last_byte;
}