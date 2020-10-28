#ifndef UART_H
#define UART_H

#include <stdio.h>

#define NEWLINE   "\r\n"

#define uart_print printf

void uart_init(void);
void uart_transmit_byte(char byte);
char uart_receive_byte(void);
int uart_transmit(void * buffer, int size);
int uart_receive(void * buffer, int size);
int uart_transmit_message(enum Message message);
enum Message uart_receive_message(void);

#endif /* UART_H */
