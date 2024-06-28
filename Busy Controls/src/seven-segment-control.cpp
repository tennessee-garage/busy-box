#include <Arduino.h>
#include "SegmentedDisplay.h"
#include "TinySPI.h"

// Serial register pins
#define SERIAL_IN_PIN PIN_A0
#define SERIAL_CLOCK_PIN PIN_A1
#define SERIAL_CLEAR_PIN PIN_A2
#define REGISTER_CLOCK_PIN PIN_A3
#define OUTPUT_ENABLE_PIN PIN_B0

#define NUM_DISPLAYS 2

// SPI pins
#define DO_PIN PIN_A5
#define DI_PIN PIN_A6
#define SCLK_PIN PIN_A4
#define SS_PIN PIN_A7

TinySPI *spi;
SegmentedDisplay *DISP;

void setup() {
    DISP = new SegmentedDisplay(SERIAL_IN_PIN,
                                SERIAL_CLOCK_PIN, 
                                SERIAL_CLEAR_PIN, 
                                REGISTER_CLOCK_PIN, 
                                OUTPUT_ENABLE_PIN, 
                                NUM_DISPLAYS);

    DISP->test_pattern();
    DISP->blank();

    spi = new TinySPI(DO_PIN, DI_PIN, SCLK_PIN, SS_PIN);
    spi->begin();
}

void loop() {
    if (spi->poll_byte()) {
        DISP->write(spi->last_byte());
    }
}
