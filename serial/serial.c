#include <stdio.h>
#include <string.h>
#include <xc.h>
#include <pic16f690.h>

#include "uart.h"
#include "serial.h"

#define null       0

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

int serial_print(char * string)
{
	return serial_printf(string);
}

int serial_println(char * string)
{
	return serial_printf(string) + serial_printf(NEWLINE);
}

int serial_read(char * input, int length)
{
	if (input == null || length < 0)
	{
		return ERROR;
	}

	char next_byte = 0;
	int byte_count = 0;
	memset(input, 0, length);

	for (int i = 0; i < length - 1; i++)
	{
		next_byte = uart_receive_byte();

		if (next_byte == '\r')   // carriage return [Enter] indicates end of transmission
		{

			break;
		}

		input[i] = next_byte;    // save new byte in input buffer
		byte_count++;
	}

	return byte_count;
}

void putch(char byte)
{
    uart_transmit_byte(byte);
}
