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
    _response_byte = 0;

    // We start DO off as an input and set to LOW, i.e. floating.  We only make
    // it an output when we're selected.  If we don't do this, at 3+ clients on
    // the bus, it becomes impossible for any one client to successfully set
    // this line. as the others are all pulling it down.
    pinMode(_do_pin, INPUT);

    pinMode(_di_pin, INPUT);
    pinMode(_enable_pin, INPUT_PULLUP);
    pinMode(_clk_pin, INPUT);

    digitalWrite(_do_pin, LOW);
}

bool TinySPI::is_chip_selected() {
    return digitalRead(_enable_pin) == 0;
}

void TinySPI::enable_spi() {
    // Re-establish DO as an output
    pinMode(_do_pin, OUTPUT);

    // Pulse the DO pin to let the host know we're ready.  Without this the 
    // host needs to wait an arbitrary amount of time hoping this device will be
    // ready by then.
    digitalWrite(_do_pin, LOW);
    digitalWrite(_do_pin, HIGH);
    digitalWrite(_do_pin, LOW);

    SPI_ON();

    // Clear overflow when we see the falling edge of the enable pin
    CLEAR_OVERFLOW();

    // Load whatever our current response byte is
    USIDR = _response_byte;
}

void TinySPI::disable_spi() {
    SPI_OFF();

    // Make sure DO is set floating, so other clients can use this shared line
    pinMode(_do_pin, INPUT);
    digitalWrite(_do_pin, LOW);
}

bool TinySPI::poll_byte() {
    bool has_received_data = false;

    if (!is_chip_selected()) {
        return has_received_data;
    }

    enable_spi();

    // Wait for bytes to be read in, at which point overflow will trigger
    while (NOT_IN_OVERFLOW() && is_chip_selected());

    if (IN_OVERFLOW()) {
        // Read the value we got in and immediately write out the value we want to send forward
        _last_byte = USIDR;
        has_received_data = true;
    }

    disable_spi();

    return has_received_data;
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