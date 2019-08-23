#include <xc.h>
#include <pic16f690.h>

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

	// BAUD RATE (9600 bps, 3.6864MHz Fosc) //

	TXSTAbits.BRGH = 1;     // use high baud rate
	BAUDCTLbits.BRG16 = 0;  // use 8-bit baud rate generator
	BAUDCTLbits.SCKP = 0;   // don't invert transmitted bits
	SPBRG = 23;             // low register BRG multiplier
	SPBRGH = 0;             // high register BRG multiplier
}

void uart_send(const char * data)
{
	for (int i = 0; data[i] != '\0'; i++)
	{
		while (TXSTAbits.TRMT == 0);   // Wait for TX register to empty
		TXREG = data[i];               // Load new byte into TX register
	}
}

void uart_receive(char * buffer, unsigned int length)
{
	char new_byte = 0;
	int i = 0;

	for (i = 0; i < length - 1; i++)
	{
		while (PIR1bits.RCIF == 0);   // Wait for RX register to receive new byte
		new_byte = RCREG;             // Read new byte from RX buffer

		if (new_byte == '\0' || new_byte == '\n' || new_byte == '\r')
		{
			break;
		}

		buffer[i] = new_byte;
	}

	buffer[i + 1] = '\0';
}
