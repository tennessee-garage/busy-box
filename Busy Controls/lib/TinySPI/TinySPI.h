#ifndef BUSY_CONTROLS_TINYSPI_H
#define BUSY_CONTROLS_TINYSPI_H

// A library to implement some simple slave SPI communications on an ATTinyX4, since
// this devices does not have dedicated SPI hardware, and is not slave-select aware.

#include "Arduino.h"

class TinySPI {
public:
    static TinySPI* instance;

    TinySPI(uint8_t do_pin, uint8_t di_pin, uint8_t clk_pin, uint8_t en_pin);
    void begin();
    bool poll_byte();
    void set_response_byte(uint8_t response);
    uint8_t last_byte();

    bool is_chip_selected();
    void enable_spi();
    void disable_spi();

private:
    uint8_t _enable_pin;
    uint8_t _do_pin, _di_pin, _clk_pin;
    uint8_t _last_byte, _response_byte;
};

#endif