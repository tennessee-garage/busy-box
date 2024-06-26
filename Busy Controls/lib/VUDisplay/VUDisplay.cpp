#include "VUDisplay.h"

VUDisplay::VUDisplay(uint8_t pin, uint8_t max) {
    _vu_pin = pin;
    _max = max;

    _scale_factor = 255.0/_max;

    pinMode(_vu_pin, OUTPUT);
}

void VUDisplay::write(uint8_t val) {
    // The gauge is inverted; writing zero shows it the full 12V source, writing 255
    // grounds it through the MOSFET, showing zero on the gauge.
    analogWrite(_vu_pin, int((_max-val) * _scale_factor));
}

uint8_t VUDisplay::value() {
    return _value;
}
