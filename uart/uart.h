#ifndef UART_H
#define UART_H

void uart_init(void);
int uart_send(const char * data);
int uart_receive(char * buffer, int length);

#endif /* UART_H */
