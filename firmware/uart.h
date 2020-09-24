#ifndef UART_H
#define UART_H

#include <stdio.h>

#define NEWLINE   "\r\n"

#define uart_print printf

void uart_init(void);
void uart_transmit_byte(char byte);
char uart_receive_byte(void);
int uart_transmit(char * data, int length);
int uart_receive(char * buffer, int length);
int uart_read(char * input, int length);
int uart_scan(char * pattern);

#endif /* UART_H */
