//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef UARTRING_H
#define UARTRING_H

#include <stdint.h>

void uart_init(unsigned long baud);
char uart_inwaiting(void);
char uart_get(void);
void uart_put(char c);
void uart_puts(const char *s);
void uart_put_block(char c);
void uart_puts_block(const char *s);
void uart_put_int(int32_t n);

#endif
