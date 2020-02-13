#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>

#define serial_printc putch
#define serial_printf printf

int serial_print(const char * string);
int serial_println(const char * string);
void putch(const char byte);

#endif /* SERIAL_H */
