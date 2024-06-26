#ifndef BUSY_CONTROLS_VUDISPLAY_H
#define BUSY_CONTROLS_VUDISPLAY_H

#include "Arduino.h"

class VUDisplay {
public:
    VUDisplay(uint8_t pin, uint8_t max);
    void write(uint8_t val);
    uint8_t value();

private:
    uint8_t _vu_pin;
    uint8_t _max;
    float _scale_factor;
    uint8_t _value;
};

#endif