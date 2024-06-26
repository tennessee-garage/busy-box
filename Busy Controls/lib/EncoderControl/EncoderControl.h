#ifndef BUSY_CONTROLS_ENCODERCONTROL_H
#define BUSY_CONTROLS_ENCODERCONTROL_H

#include "Arduino.h"

#define BUTTON_DEBOUNCE_MS 5

class EncoderControl {
public:
    EncoderControl(uint8_t data_pin, uint8_t clock_pin, uint8_t button_pin, uint16_t max_value);
    void poll();
    uint16_t current_value();
    bool was_button_pressed();
    void clear_button_press();

private:
    uint8_t _data_pin, _clock_pin, _button_pin;
    uint8_t _prev_next_code = 0;
    uint16_t _store = 0;

    uint16_t _current_value, _max_value;

    // Keep track of what state we've read from the button
    bool _button_down = false;
    // If the button was pressed, track it here in a clearable flag (i.e. save it for things to 
    // act on, even if the button is no longer pressed by the time they read it)
    bool _button_pressed_flag = false;

    unsigned long _last_button_change = 0;

    uint8_t _read_rotary();
    bool _button_reports_pressed();
};

#endif