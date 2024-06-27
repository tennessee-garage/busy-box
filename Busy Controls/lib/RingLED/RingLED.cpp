#include <Arduino.h>
#include "RingLED.h"

RingLED::RingLED(uint16_t num_leds) : RingLED(num_leds, DEFAULT_BRIGHTNESS) {}

RingLED::RingLED(uint16_t num_leds, uint8_t brightness) {
    _num_leds = num_leds;
    _leds = new CRGB[num_leds];

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(_leds, num_leds);
    FastLED.setBrightness(brightness);
    clear();
}

void RingLED::clear() {
    for (uint16_t i=0; i < _num_leds; i++) {
        _leds[i] = CRGB::Black;
    }
    FastLED.show();
}

void RingLED::test_pattern() {
    uint8_t hue = 0;
    uint8_t sat = 255;
    uint8_t val = 255;
  
    for (uint16_t i=0; i < _num_leds; i++) {
        _leds[i] = CHSV(hue, sat, val);
        hue += (int) 255.0/_num_leds;
        
        FastLED.show();
        delay(200);
    }
}

void RingLED::set_display_method(DisplayMethod method) {
    _display_method = method;
}

void RingLED::display(uint16_t value) {
    switch(_display_method) {
        case RedDotUpdate:
            _display_red_dot(value);
            break;
        case GreenDotUpdate:
            _display_green_dot(value);
            break;
        case DoubleIndicator:
            _display_double(value);
            break;
        case FadeInUpdate:
            _display_fade_in(value);
            break;
        case StackingUpdate:
            _display_stacking(value);
            break;
    }

    FastLED.show();
}

void RingLED::_display_red_dot(uint16_t value) {
    for (uint16_t i=0; i < _num_leds; i++) {
        if (i == value % _num_leds) {
            _leds[i] = CRGB::Red;
        } else {
            _leds[i] = CRGB::Blue;
        }
    }
}

void RingLED::_display_green_dot(uint16_t value) {
    for (uint16_t i=0; i < _num_leds; i++) {
        if (i == value % _num_leds) {
            _leds[i] = CRGB::Green;
        } else {
            _leds[i] = CRGB::Black;
        }
    }
}

void RingLED::_display_double(uint16_t value) {
    uint8_t hue = 170;
    uint8_t sat = 255;
    uint8_t val_bkg = 50;
    uint8_t val_ind = 150;
  
    for (uint16_t i=0; i < _num_leds; i++) {
        if (i == value % _num_leds) {
            _leds[i] = CHSV(hue, sat, val_ind);
        } else {
            _leds[i] = CHSV(hue, sat, val_bkg);
        }
    }
    for (uint16_t i=0; i < _num_leds; i++) {
        if (i == (value/(_num_leds+1))%_num_leds) {
            _leds[i] = CRGB::Green;
        }
    }
}

void RingLED::_display_fade_in(uint16_t value) {
    uint8_t hue = 66;
    uint8_t sat = 255;
    
    for (uint16_t i=0; i < _num_leds; i++) {
        if (i <= value % _num_leds) {
            _leds[i] = CHSV(hue, sat, (int) 255.0*(i/_num_leds)); //CRGB::Red;
        } else {
            _leds[i] = CRGB::Black;
        }
    }
}

void RingLED::_display_stacking(uint16_t value) {
    uint8_t hue = 66;
    uint8_t sat = 255;
    
    for (uint16_t i=0; i < _num_leds; i++) {
        if (i <= value % _num_leds) {
            _leds[i] = CHSV(hue, sat, (int) 255.0*(i/_num_leds)); //CRGB::Red;
        } else {
            _leds[i] = CRGB::Black;
        }
    }
}
