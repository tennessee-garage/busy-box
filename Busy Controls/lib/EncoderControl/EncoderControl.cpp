#include "EncoderControl.h"

EncoderControl::EncoderControl(uint8_t data_pin, uint8_t clock_pin, uint8_t button_pin, uint16_t max_value) {
    _data_pin = data_pin;
    _clock_pin = clock_pin;
    _button_pin = button_pin;
    _current_value = 0;
    _max_value = max_value;

    pinMode(_data_pin, INPUT);
    pinMode(_clock_pin, INPUT);
    pinMode(_button_pin, INPUT);

    // Set the pullup resistor on the button pin
    digitalWrite(_button_pin, HIGH);
}

void EncoderControl::poll() {
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
    
    _current_value = (_current_value + _read_rotary()) % (_max_value + 1);
    if (_current_value < 0) {
        _current_value = 0;
    }
}

uint16_t EncoderControl::current_value() {
    return _current_value;
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

uint8_t EncoderControl::_read_rotary() {
    static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

    _prev_next_code <<= 2;
    if (digitalRead(_data_pin)) _prev_next_code |= 0x02;
    if (digitalRead(_clock_pin)) _prev_next_code |= 0x01;
    _prev_next_code &= 0x0f;

    // If valid then store as 16 bit data.
    if (rot_enc_table[_prev_next_code] ) {
        _store <<= 4;
        _store |= _prev_next_code;
        if ((_store & 0xff) == 0x2b) return -1;
        if ((_store & 0xff) == 0x17) return 1;
    }

    return 0;
}