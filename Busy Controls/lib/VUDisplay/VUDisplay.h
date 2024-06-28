#ifndef BUSY_CONTROLS_VUDISPLAY_H
#define BUSY_CONTROLS_VUDISPLAY_H

// Controls a VU meter such as: https://www.aliexpress.us/item/2251832764045918.html
// The device itself is really only a voltage meter (i.e. it does not have control circuitry
// to set it arbitrarily), so this library is controlling this board (https://oshpark.com/projects/YezPrUKP/view_design)
// which sends out the appropriate voltage to attain the desired reading on the VU.

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