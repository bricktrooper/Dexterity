#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_send(const char * data);
void uart_receive(void);

#endif /* UART_H */
