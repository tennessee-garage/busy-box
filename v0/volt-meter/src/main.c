/**
 * Code to drive an ATTiny861a
 *
 * http://www.atmel.com/Images/doc8197.pdf
 *
 * - Drives an RGB LED Strip via PWM
 * - Reads 4 load sensors in a wheatstone bridge configuration via differential ADC
 * - Communicates to a master controller via i2c
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "main.h"

// Clear the overflow flag after reading data from an SPI transfer
#define CLEAR_OVERFLOW() USISR |= _BV(USIOIF)

// Test whether the overflow condition has happened
#define IN_OVERFLOW() (USISR & _BV(USIOIF))
#define NOT_IN_OVERFLOW() !IN_OVERFLOW()

// Read the value off the chip select
#define CHIP_SELECT() (PINA & _BV(PINA3))
#define IS_CHIP_SELECTED() (CHIP_SELECT() == 0)

// Pull 10-bit values
#define DECODE_VM3(byte1, byte2) ((((byte1) & 0x3F) << 4) | ((byte2) >> 4))
#define DECODE_VM2(byte1, byte2) ((((byte1) & 0x0F) << 6) | ((byte2) >> 2))
#define DECODE_VM1(byte1, byte2) ((((byte1) & 0x03) << 8) | (byte2))

#define SET_VOLTAGE1(val) set_vm1((val/10)*1023)
#define SET_VOLTAGE2(val) set_vm2((val/10)*1023)
#define SET_VOLTAGE3(val) set_vm3((uint16_t)(((10.0-val)/10.0)*1023))

// Each packet transmitted has 4 bytes.  The dance floor has 64 squares so will need
// to have 64 packets sent to set LED values.  Using 96 to give some extra room
#define DATA_BYTES 4
#define MAX_PACKETS 96

// Hold the data being piped through the squares
uint8_t buffer[DATA_BYTES * MAX_PACKETS];

void init_avr() {
    // Set all of port A to input
    DDRA = 0x00;

    // set all of port B to output
    DDRB = 0xFF;
    PORTB = 0x00;
}

void init_pwm(void) {

    // Set low speed mode (LSM) and start the PLL (PLLE)
    PLLCSR = _BV(LSM) | _BV(PLLE);

    while (!(PLLCSR & 1<<PLOCK));

    // Enable PCKE (set all rather than set bit via |= to save space)
    PLLCSR = _BV(LSM) | _BV(PCKE) | _BV(PLLE);

    // COM1A1 COM1A0 COM1B1 COM1B0 FOC1A FOC1B PWM1A PWM1B
    TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(PWM1A) | _BV(PWM1B);

    // prescaler at 64
    TCCR1B = _BV(CS12) | _BV(CS11) | _BV(CS10);

    // COM1A1S COM1A0S COM1B1S COM1B0S COM1D1 COM1D0 FOC1D PWM1D
    TCCR1C = _BV(COM1A1S) | _BV(COM1B1S) | _BV(COM1D1) | _BV(PWM1D);

    // FPIE1 FPEN1 FPNC1 FPES1 FPAC1 FPF1 WGM11 WGM10
    TCCR1D = _BV(WGM10);

    // Clear the compare counter
    TC1H = 0x00;
    TCNT1 = 0x00;

    // Set the top value of the compare count to 1023
    TC1H = 0x03;
    OCR1C = 0xFF;

    // Timer/Counter0 Control Register B: CS01 & CS00 == clock divided by 64
    TCCR0B = _BV(CS01) | _BV(CS00);

    TCNT0H = 0x00;
    TCNT0L = 0x00;
}

void test_vm(int num) {
    void (*func)(uint16_t);

    if (num == 1) {
        func = set_vm1;
    } else if (num == 2) {
        func = set_vm2;
    } else {
        func = set_vm3;
    }

    set_vm1(0);
    set_vm2(0);
    set_vm3(0);

    for (int i=0; i <= 10; i++) {
        func((uint16_t) (1023 / 10) * i);
        _delay_ms(2000);
    }
}

void init_spi(void) {
    // USIWM0 sets 3 wire mode, USICS1 sets an external clock source
    USICR = _BV(USIWM0) | _BV(USICS1);

    // Set alternate SPI pins on PORTA (PA0=DI, PA1=DO, PA2=USCK)
    USIPP = _BV(USIPOS);

    // Set PA0 to be an output for the DO SPI function
    DDRA |= _BV(DDA1);

    for (int i = 0; i < DATA_BYTES * MAX_PACKETS; i++) {
        buffer[i] = 0x00;
    }

    // Clear the overflow flag
    CLEAR_OVERFLOW();
}

void set_vm1(uint16_t val) {
    TC1H = val >> 8;
    OCR1D = val & 0x0FF;
}

void set_vm2(uint16_t val) {
    TC1H = val >> 8;
    OCR1A = val & 0x0FF;
}

void set_vm3(uint16_t val) {
    TC1H = val >> 8;
    OCR1B = val & 0x0FF;
}

void handle_spi(void) {
    uint8_t transferred = 0;
    uint8_t val_in;

    // Start the head at 4 since 0..3 are the weight values
    uint16_t head = 4;

    // When we enter this method USIDR already has buffer[head-4] loaded.  When we
    // write out val_out it will be for the next byte, which is buffer[head+1-4] or buffer[head-3]
    uint8_t val_out = buffer[head-3];

    // Wait for the next cycle to start
    while (!IS_CHIP_SELECTED());

    // While in chip select, read data
    while (IS_CHIP_SELECTED()) {
        transferred = 1;

        // Wait for bytes to be read in, at which point overflow will trigger
        while (NOT_IN_OVERFLOW() && IS_CHIP_SELECTED());

        // Read the value we got in and immediately write out the value we want to send forward
        val_in = USIDR;
        USIDR = val_out;
        CLEAR_OVERFLOW();

        // Write the value we got to the head of the fifo
        buffer[head] = val_in;
        head++;

        // Set the value we'll output on the next round.  Now that head has been incremented, the
        // value in USIDR is buffer[head-4].  To ready the next value we grab buffer[head+1-4]
        val_out = buffer[head-3];
    }

    // If we didn't just transfer some bytes, don't decode and set LEDs
    if (!transferred) {
        return;
    }

    // The value of head is incremented after the last byte is written, and then incremented again when
    // chip select goes high (not selected) for this reason the last byte written is at head - 2
    uint16_t vm3 = DECODE_VM3(buffer[head - 5], buffer[head - 4]);
    uint16_t vm2 = DECODE_VM2(buffer[head - 4], buffer[head - 3]);
    uint16_t vm1 = DECODE_VM1(buffer[head - 3], buffer[head - 2]);

    set_vm2(vm3);
    set_vm3(vm2);
    set_vm1(vm1);
}

void sync_spi(void) {
    // Wait out a full cycle to make sure we are fully synced up

    // Wait for any current cycle to finish
    while (IS_CHIP_SELECTED());

    // Wait for the next cycle to start
    while (!IS_CHIP_SELECTED());

    // Wait out this cycle
    while (IS_CHIP_SELECTED());
}

int main(void) {
    init_avr();
    init_pwm();
    //init_spi();

    // Test each voltmeter
    //test_vm(1);
    //test_vm(2);
    test_vm(3);

    // Clear the voltmeters
    //set_vm3(0);
    //set_vm1(0);
    //set_vm2(0);

    // Get ourselves synced up by waiting out one cycle
    //sync_spi();

    while (1) {
        // Test each voltmeter
        //test_vm(1);
        //test_vm(2);
        //test_vm(3);
        //handle_spi();
        SET_VOLTAGE3(6);
        _delay_ms(2000);
        SET_VOLTAGE3(10);
        _delay_ms(2000);
    }
}
