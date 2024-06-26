#ifndef BUSY_CONTROLS_SEGMENTEDDISPLAY_H
#define BUSY_CONTROLS_SEGMENTEDDISPLAY_H

#include "Arduino.h"

class SegmentedDisplay {
public:
    SegmentedDisplay(uint8_t serial_in_pin, 
                     uint8_t serial_clock_pin, 
                     uint8_t serial_clear_pin, 
                     uint8_t register_clock_pin, 
                     uint8_t output_enable_pin, 
                     uint8_t num_displays);

    void write(int val);
    void blank();

private:
    uint8_t _sr_in_pin, _sr_clk_pin, _sr_clr_pin, _reg_clk_pin, _oe_pin;
    uint8_t _num_displays;

    uint8_t _numeric_segments[10][8] = {
      // A, B, C, D, E, F, G, -
        {1, 1, 1, 1, 1, 1, 0, 0}, // 0
        {0, 1, 1, 0, 0, 0, 0, 0}, // 1
        {1, 1, 0, 1, 1, 0, 1, 0}, // 2
        {1, 1, 1, 1, 0, 0, 1, 0}, // 3
        {0, 1, 1, 0, 0, 1, 1, 0}, // 4
        {1, 0, 1, 1, 0, 1, 1, 0}, // 5
        {1, 0, 1, 1, 1, 1, 1, 0}, // 6
        {1, 1, 1, 0, 0, 0, 0, 0}, // 7
        {1, 1, 1, 1, 1, 1, 1, 0}, // 8
        {1, 1, 1, 1, 0, 1, 1, 0}  // 9
    };

    uint8_t _blank[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    void _clear_register();
    void _enable_register();
    void _disable_register();
    void _serial_high();
    void _serial_low();
    void _pulse_serial_clock();
    void _pulse_register_clock();
    void _load_register(uint8_t *buf);
};

#endif