// screen tty.usbserial-AL065BVB 9600
// (ctrl + a + k) ---> y

#include <xc.h>
#include <pic16f690.h>

#include <string.h>

#include "utils.h"

#include "uart.h"

void uart_init(void)
{
	// BASIC CONFIG //

	TXSTAbits.SYNC = 0;     // use asynchronous mode
	RCSTAbits.SPEN = 1;     // enable serial port pins (TX = OUTPUT, RX = INPUT)

	// TRANSMITTER (TX) //

	TXSTAbits.TXEN = 1;     // enable TX circuitry
	TXSTAbits.TX9 = 0;      // use 8-bit transmission

	// RECEIVER (RX) //

	RCSTAbits.CREN = 1;     // enable RX circuitry
	RCSTAbits.RX9 = 1;      // use 8-bit reception

	// BAUD RATE //

	// 9600 bps @ 3.6864MHz Fosc
	// 10417 bps @ 4MHz Fosc

	TXSTAbits.BRGH = 1;     // use high baud rate
	BAUDCTLbits.BRG16 = 0;  // use 8-bit baud rate generator
	BAUDCTLbits.SCKP = 0;   // don't invert transmitted bits
	SPBRG = 23;             // low register BRG multiplier
	SPBRGH = 0;             // high register BRG multiplier
}

void uart_transmit_byte(char byte)
{
	while (TXSTAbits.TRMT == 0);   // wait for TX register to empty
	TXREG = byte;                  // load new byte into TX register
}

char uart_receive_byte(void)
{
	while (PIR1bits.RCIF == 0);   // wait for RX register to receive new byte
	return RCREG;                 // read new byte from RX buffer
}

int uart_transmit(char * data, int size)
{
	if (data == NULL || size < 0)
	{
		return ERROR;
	}

	int count = 0;

	for (int i = 0; i < size; i++)
	{
		uart_transmit_byte(data[i]);
		count++;
	}

	return count;
}

int uart_receive(char * data, int size)
{
	if (data == NULL || size < 0)
	{
		return ERROR;
	}

	int count = 0;

	for (int i = 0; i < size; i++)
	{
		data[i] = uart_receive_byte();
		count++;
	}

	return count;
}

int uart_scan(char * data, int size)
{
	if (data == NULL || size < 0)
	{
		return ERROR;
	}

	char next = 0;
	int count = 0;
	memset(data, 0, size);

	for (int i = 0; i < size - 1; i++)
	{
		next = uart_receive_byte();

		if (next == '\r')   // carriage return [Enter] indicates end of transmission
		{
			break;
		}

		data[i] = next;    // save new byte in input buffer
		count++;
	}

	return count;
}

void putch(char byte)
{
    uart_transmit_byte(byte);
}
