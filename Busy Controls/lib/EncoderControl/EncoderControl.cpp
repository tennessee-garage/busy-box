#include "EncoderControl.h"

EncoderControl::EncoderControl(uint8_t data_pin, uint8_t clock_pin, uint8_t button_pin, uint16_t max_value) {
    _data_pin = data_pin;
    _clock_pin = clock_pin;
    _button_pin = button_pin;
    _current_value = 0;
    _max_value = max_value;

    pinMode(_data_pin, INPUT_PULLUP);
    pinMode(_clock_pin, INPUT_PULLUP);
    pinMode(_button_pin, INPUT_PULLUP);
}

bool EncoderControl::poll() {
    if (!_button_down &&
        _button_reports_pressed() &&
        ((millis() - _last_button_change) > BUTTON_DEBOUNCE_MS)) {
        _last_button_change = millis();
        _button_down = true;
        _button_pressed_flag = true;
    }

    if (!_button_reports_pressed()) {
        _button_down = false;
        _last_button_change = millis();
    }

    int8_t rotary_incr = _read_rotary();
    _current_value += rotary_incr;

    // Hold the resulting value at our defined bounds
    if (_current_value < 0) {
        _current_value = 0;
    } else if (_current_value > _max_value) {
        _current_value = _max_value;
    }

    bool has_new_value = false;
    // If we detect a rotation, then we have a new value
    if (rotary_incr != 0)  {
        has_new_value = true;
    }
    // If the value was changed externally (via set_value) then we have a new value
    if (_value_changed_externally) {
        _value_changed_externally = false;
        has_new_value = true;
    }

    return has_new_value;
}

uint16_t EncoderControl::current_value() {
    return _current_value;
}

void EncoderControl::clear_value() {
    set_value(0);
}

void EncoderControl::set_value(uint8_t value) {
    _value_changed_externally = true;
    _current_value = value;
}

bool EncoderControl::was_button_pressed() {
    return _button_pressed_flag;
}

void EncoderControl::clear_button_press() {
    _button_pressed_flag = false;
}

bool EncoderControl::_button_reports_pressed() {
    return digitalRead(_button_pin) == 0;
}

// source: https://www.best-microcontroller-projects.com/rotary-encoder.html
int8_t EncoderControl::_read_rotary() {
    static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

    _prev_next_code <<= 2;
    if (digitalRead(_data_pin)) _prev_next_code |= 0x02;
    if (digitalRead(_clock_pin)) _prev_next_code |= 0x01;
    _prev_next_code &= 0x0f;

    // If valid then store as 16 bit data.
    if (rot_enc_table[_prev_next_code] ) {
        _store <<= 4;
        _store |= _prev_next_code;
        // Altered from source, 0x2b seemed incorrect based on the writeup
        //if ((_store & 0xff) == 0x2b) return -1;
        if ((_store & 0xff) == 0xbd) return -1;
        if ((_store & 0xff) == 0x17) return 1;
    }

    return 0;
}