#include "TinySPI.h"

// Clear the overflow flag after reading data from an SPI transfer
#define CLEAR_OVERFLOW() USISR |= _BV(USIOIF)

// Test whether the overflow condition has happened
#define IN_OVERFLOW() (USISR & _BV(USIOIF))
#define NOT_IN_OVERFLOW() !IN_OVERFLOW()

// USIWM0 sets 3 wire mode, USICS1 sets an external clock source
#define SPI_ON_SETTINGS _BV(USIWM0) | _BV(USICS1)
#define SPI_OFF_SETTINGS 0

// Use these to enable and disable SPI.  Since there's no native slave select handling, we want to make sure
// our device doesn't send the data sitting in USIDR down the line when its some other devices turn to communicate
#define SPI_ON() USICR = SPI_ON_SETTINGS
#define SPI_OFF() USICR = SPI_OFF_SETTINGS

TinySPI* TinySPI::instance;

TinySPI::TinySPI(uint8_t do_pin, uint8_t di_pin, uint8_t clk_pin, uint8_t en_pin) {
    _do_pin = do_pin;
    _di_pin = di_pin;
    _clk_pin = clk_pin;
    _enable_pin = en_pin;

    // Set the data out pin to INPUT to keep it floating until we're actually selected.
    pinMode(_do_pin, INPUT);

    pinMode(_di_pin, INPUT);
    pinMode(_enable_pin, INPUT_PULLUP);
    pinMode(_clk_pin, INPUT);
}

void TinySPI::begin() {
    // Enable pin change interrupt on tne enable pin
    GIMSK |= (1 << PCIE0);

    // Enable appropriate interrupt for the given select pin 
    switch (_enable_pin) {
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

    if (TinySPI::instance->is_chip_selected()) {
        TinySPI::instance->enable_spi();
    }
}

bool TinySPI::is_chip_selected() {
    return digitalRead(_enable_pin) == 0;
}

void TinySPI::enable_spi() {
        pinMode(_do_pin, OUTPUT);
        SPI_ON();

        // Clear overflow when we see the falling edge of the enable pin
        CLEAR_OVERFLOW();

        // Load whatever our current response byte is
        USIDR = _response_byte;
}

void TinySPI::disable_spi() {
    pinMode(_do_pin, INPUT);
    SPI_OFF();
}

bool TinySPI::poll_byte() {
    if (!is_chip_selected()) {
        return false;
    }

    // Wait for bytes to be read in, at which point overflow will trigger
    while (NOT_IN_OVERFLOW());

    // Read the value we got in and immediately write out the value we want to send forward
    _last_byte = USIDR;

    disable_spi();

    return true;
}

// This value gets shifted off as the response while data from the master is comming in,
// so we have to set it ahead of time.  This cannot be used for values that respond to
// data the master sends 
void TinySPI::set_response_byte(uint8_t response) {
    _response_byte = response;
}

uint8_t TinySPI::last_byte() {
    return _last_byte;
}