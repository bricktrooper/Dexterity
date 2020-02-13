#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <xc.h>
#include <pic16f690.h>

#include "../uart/uart.h"
#include "serial.h"

#define null       0

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

#define NEWLINE   "\r\n"

int serial_print(const char * string)
{
	return serial_printf(string);
}

int serial_println(const char * string)
{
	return serial_printf(string) + serial_printf(NEWLINE);
}

void putch(const char byte)
{
    uart_transmit(&byte, 1);
}
