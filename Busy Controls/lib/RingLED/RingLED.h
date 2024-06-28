#ifndef BUSY_CONTROLS_RINGLED_H
#define BUSY_CONTROLS_RINGLED_H

// This library controls a neopixel ring: https://www.adafruit.com/product/1643
// It is able to take a variable number of pixels, so that it should be able to work with 
// the 12 or 16 light ring.  For variouis reasons (see next comment below) the pin can't be passed in.

#include <Arduino.h>
#include <FastLED.h>

// For bizzare reasons (https://github.com/FastLED/FastLED/issues/1137) the pin must be constant.  On an ATTiny84
// PIN_PB0 experimentally maps to a value of 10.  Likely for similar reasons as the above github issue 
// (though I can't find a definitive answer) you can't pass in the pin constant (PIN_PB0), and anyway, its
// value does not resolve to 10.  No idea how this  mapping works.
#define DATA_PIN 10

// The brightness ranges from 0-255.
#define DEFAULT_BRIGHTNESS 64

enum DisplayMethod {
    RedDotUpdate,
    GreenDotUpdate,
    DoubleIndicator,
    FadeInUpdate,
    StackingUpdate
};

class RingLED
{
  public:
    RingLED(uint16_t num_leds);
    RingLED(uint16_t num_leds, uint8_t brightness);
    
    void clear();
    void test_pattern();
    void set_display_method(DisplayMethod method);
    void display(uint16_t value);

  private:
    uint16_t _num_leds;
    CRGB *_leds;

    DisplayMethod _display_method;

    void _display_red_dot(uint16_t value);
    void _display_green_dot(uint16_t value);
    void _display_double(uint16_t value);
    void _display_fade_in(uint16_t value);
    void _display_stacking(uint16_t value);
};
#endif