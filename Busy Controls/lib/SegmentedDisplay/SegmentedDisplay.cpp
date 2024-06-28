#include "SegmentedDisplay.h"

SegmentedDisplay::SegmentedDisplay(uint8_t serial_in_pin,
                                   uint8_t serial_clock_pin,
                                   uint8_t serial_clear_pin,
                                   uint8_t register_clock_pin, 
                                   uint8_t output_enable_pin, 
                                   uint8_t num_displays) {

    _sr_in_pin = serial_in_pin;
    _sr_clk_pin = serial_clock_pin;
    _sr_clr_pin = serial_clear_pin;
    _reg_clk_pin = register_clock_pin;
    _oe_pin = output_enable_pin;
    _num_displays = num_displays;
                                 
    pinMode(_sr_in_pin, OUTPUT);
    pinMode(_sr_clk_pin, OUTPUT);
    pinMode(_sr_clr_pin, OUTPUT);
    pinMode(_reg_clk_pin, OUTPUT);
    pinMode(_oe_pin, OUTPUT);

    _enable_register();
    _clear_register();

    _serial_low();
    digitalWrite(_sr_clk_pin, LOW);
    digitalWrite(_reg_clk_pin, LOW);
}


void SegmentedDisplay::_clear_register() {
    // This pin is active low
    digitalWrite(_sr_clr_pin, LOW);
    digitalWrite(_sr_clr_pin, HIGH);
}

void SegmentedDisplay::_enable_register() {
    // This pin is active low
    digitalWrite(_oe_pin, LOW);
}

void SegmentedDisplay::_disable_register() {
    // This pin is active low
    digitalWrite(_oe_pin, HIGH);
}

void SegmentedDisplay::_serial_high() {
    digitalWrite(_sr_in_pin, HIGH);
}

void SegmentedDisplay::_serial_low() {
    digitalWrite(_sr_in_pin, LOW);
}

void SegmentedDisplay::_pulse_serial_clock() {
    digitalWrite(_sr_clk_pin, HIGH);
    digitalWrite(_sr_clk_pin, LOW);
}

void SegmentedDisplay::_pulse_register_clock() {
    digitalWrite(_reg_clk_pin, HIGH);
    digitalWrite(_reg_clk_pin, LOW);
}

void SegmentedDisplay::_load_register(uint8_t *buf) {
    for (int i=7; i >= 0; i--) {
        if (buf[i]) {
            _serial_high();
        } else {
            _serial_low();
        }
        _pulse_serial_clock();
    }
    
    _serial_low();
}

void SegmentedDisplay::write(int val) {
    int display = 1;
    int digit;
    while (display <= _num_displays) {
        digit = val % 10;
        _load_register(_numeric_segments[digit]);
        display++;
        val /= 10;
    }
    _pulse_register_clock();
}

void SegmentedDisplay::blank() {
    int display = 1;
    while (display <= _num_displays) {
        _load_register(_blank);
        display++;
    }
    _pulse_register_clock();
}

void SegmentedDisplay::test_pattern() {
    blank();

    // Run through the test sequence 4 times
    for (int num=0; num < 4; num++) {
        // Go through each step of the sequence in order
        for (int seq=0; seq < 8; seq++) {
            // Load the same sequence of the test pattern on all displays
            for (int display=0; display < _num_displays; display++) {
                _load_register(_test_segments[seq]);
            }
            _pulse_register_clock();
            delay(100);
        }
    }
}