#include <Arduino.h>

#include "TinySPI.h"
#include "VUDisplay.h"

#define VU_PIN PIN_B2

#define DO_PIN PIN_A5
#define DI_PIN PIN_A6
#define SCLK_PIN PIN_A4
#define SS_PIN PIN_A7

TinySPI *SPI;
VUDisplay *vu;

enum commands {
    WRITE=0xA1
};

void setup() {
    SPI = new TinySPI(DO_PIN, DI_PIN, SCLK_PIN, SS_PIN);
    vu = new VUDisplay(VU_PIN, 100);

    SPI->begin();
}

void loop() {
    if (SPI->poll_byte()) {
        vu->write(SPI->last_byte());
    }
}
