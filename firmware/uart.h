#ifndef UART_H
#define UART_H

#include <stdio.h>

#define NEWLINE   "\r\n"

#define uart_print printf

void uart_init(void);
void uart_transmit_byte(char byte);
char uart_receive_byte(void);
int uart_transmit(char * data, int size);
int uart_receive(char * data, int size);
int uart_scan(char * data, int size);

#endif /* UART_H */
