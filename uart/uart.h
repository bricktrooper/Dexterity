#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_transmit_byte(const char byte);
char uart_receive_byte(void);
int uart_transmit_data(const char * data, int length);
int uart_receive_data(char * buffer, int length);

#endif /* UART_H */
