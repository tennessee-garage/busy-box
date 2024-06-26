#ifndef BUSY_CONTROLS_TINYSPI_H
#define BUSY_CONTROLS_TINYSPI_H

#include "Arduino.h"

class TinySPI {
public:
    static TinySPI* instance;
    uint8_t enable_pin;

    TinySPI(uint8_t do_pin, uint8_t di_pin, uint8_t clk_pin, uint8_t en_pin);
    void begin();
    bool poll_byte();
    uint8_t last_byte();

private:
    uint8_t _do_pin, _di_pin, _clk_pin;
    uint8_t _last_byte;

    bool _is_chip_selected();
};

#endif