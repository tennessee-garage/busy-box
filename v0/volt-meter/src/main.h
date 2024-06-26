#ifndef _MAIN_H
#define _MAIN_H

void init_avr(void);
void init_pwm(void);
void test_vm(int num);
void set_vm1(uint16_t val);
void set_vm2(uint16_t val);
void set_vm3(uint16_t val);
void handle_spi(void);
void sync_spi(void);
int main(void);

#endif
