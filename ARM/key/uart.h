#ifndef __UART_H__
#define __UART_H__

void uart_init(void);
void led_init(void);
void led(unsigned char con,unsigned char ctr);
unsigned char get_led_flag(void);

void putc(char c);
void puts(const char *p);
#endif

