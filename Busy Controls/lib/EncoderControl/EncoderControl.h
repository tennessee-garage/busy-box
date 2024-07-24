#ifndef BUSY_CONTROLS_ENCODERCONTROL_H
#define BUSY_CONTROLS_ENCODERCONTROL_H

// This library interfaces with a HW-040 rotary encoder: https://www.aliexpress.us/item/3256806292109542.html
// It stores the current value of the encoder (which is assumed to be zero at startup) and whether the button
// has been pressed.  Button presses must be polled via 'was_button_pressed', and once the caller has read
// this button press, it is responsible for clearing the flag via 'clear_button_press'.  Ther is no flag for
// whether the value has changed, its simply available via 'current_value'.

#include <Arduino.h>

#define BUTTON_DEBOUNCE_MS 5

class EncoderControl {
public:
    EncoderControl(uint8_t data_pin, uint8_t clock_pin, uint8_t button_pin, uint16_t max_value);
    bool poll();
    uint16_t current_value();
    void clear_value();
    void set_value(uint8_t);
    bool was_button_pressed();
    void clear_button_press();

private:
    uint8_t _data_pin, _clock_pin, _button_pin;
    uint8_t _prev_next_code = 0;
    uint16_t _store = 0;

    int16_t _current_value;
    uint16_t _max_value;

    // Keep track of what state we've read from the button
    bool _button_down = false;
    // If the button was pressed, track it here in a clearable flag (i.e. save it for things to 
    // act on, even if the button is no longer pressed by the time they read it)
    bool _button_pressed_flag = false;

    unsigned long _last_button_change = 0;

    bool _value_changed_externally = false;

    int8_t _read_rotary();
    bool _button_reports_pressed();
};

#endif