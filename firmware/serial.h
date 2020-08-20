#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>

#define NEWLINE   "\r\n"

#define serial_printc putch
#define serial_printf printf

int serial_print(char * string);
int serial_println(char * string);
int serial_scan(char * input, int length);
int serial_read(char * input, int length);
void putch(char byte);

#endif /* SERIAL_H */
