#include <Arduino.h>
#include <avr/io.h>
#include "VUDisplay.h"

#define VU_PIN PIN_B2

// Pins (e.g. PIN_Ax) refer to an pin on the device, vs PAx which refers to a bit
// on that port (A, B, etc)
#define DO_PIN PIN_A5
#define DI_PIN PIN_A6
#define SCLK_PIN PIN_A4
#define SS_BIT PA7
#define SS_PIN PIN_A7

// Clear the overflow flag after reading data from an SPI transfer
#define CLEAR_OVERFLOW() USISR |= _BV(USIOIF)

// Test whether the overflow condition has happened
#define IN_OVERFLOW() (USISR & _BV(USIOIF))
#define NOT_IN_OVERFLOW() !IN_OVERFLOW()

// Read the value off the chip select
#define CHIP_SELECT() (PINA & _BV(SS_BIT))
#define IS_CHIP_SELECTED() (CHIP_SELECT() == 0)

#define T0_H digitalWrite(PIN_A0, HIGH)
#define T0_L digitalWrite(PIN_A0, LOW)

#define T1_H digitalWrite(PIN_A1, HIGH)
#define T1_L digitalWrite(PIN_A1, LOW)

#define T2_H digitalWrite(PIN_A2, HIGH)
#define T2_L digitalWrite(PIN_A2, LOW)

#define T3_H digitalWrite(PIN_A3, HIGH)
#define T3_L digitalWrite(PIN_A3, LOW)

VUDisplay *vu;

void init_spi() {
    pinMode(DO_PIN, OUTPUT);

    pinMode(DI_PIN, INPUT);
    pinMode(SS_PIN, INPUT);
    pinMode(SCLK_PIN, INPUT);

    // USIWM0 sets 3 wire mode, USICS1 sets an external clock source
    USICR = _BV(USIWM0) | _BV(USICS1);

    // Clear the overflow flag
    CLEAR_OVERFLOW();

    // Make sure that we clear the overflow as soon as our chip is selectced.  This makes
    // sure that we can adjust to any weirdness from the host (e.g. new code is uploaded).
    // With out this, the whole system needs to be power cycled to get back in sync.
    //attachInterrupt(digitalPinToInterrupt(SS_PIN), []() { T2_H; CLEAR_OVERFLOW(); }, FALLING);
}

uint8_t val_in;

bool receive_byte() {
    bool data_received = false;

   // While in chip select, read data
    while (IS_CHIP_SELECTED()) {
        // Wait for bytes to be read in, at which point overflow will trigger
        while (NOT_IN_OVERFLOW());

        // Read the value we got in and immediately write out the value we want to send forward
        val_in = USIDR;
        USIDR = val_in + 1;
        data_received = true;
    }

    return data_received;
}

void setup_interrupt() {
  // Configure PA7 as input
  DDRA &= ~(1 << PA7);
  
  // Enable pull-up resistor on PA7 (optional, depending on your circuit)
  PORTA |= (1 << PA7);

  // Enable pin change interrupt on PA7
  GIMSK |= (1 << PCIE0);    // Enable pin change interrupt for port A
  PCMSK0 |= (1 << PCINT7);  // Enable interrupt for PA7

  // Enable global interrupts
  sei();
}

ISR(PCINT0_vect) {
    if (!(PINA & (1 << PA7))) {
        // Interrupt service routine code for falling edge on PA7
        CLEAR_OVERFLOW();
    }
}

void setup() {
    vu = new VUDisplay(VU_PIN, 100);

    pinMode(PIN_A0, OUTPUT);
    pinMode(PIN_A1, OUTPUT);
    pinMode(PIN_A2, OUTPUT);
    pinMode(PIN_A3, OUTPUT);
    
    T0_L;
    T1_L;
    T2_L;
    T3_L;

    setup_interrupt();
    init_spi();
}

void loop() {
    if (receive_byte()) {
        vu->write(val_in);
    }
}
