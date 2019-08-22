#include <xc.h>
#include <pic16f690.h>

#include "uart.h"

void uart_init(void)
{
	RCSTAbits.SPEN = 1;     // enable serial port pins (TX = OUTPUT, RX = INPUT)

	// TRANSMITTER (TX) //

	TXSTAbits.TX9 = 0;      // use 8-bit transmission
	TXSTAbits.TXEN = 1;     // enable TX circuitry
	TXSTAbits.SYNC = 0;     // use asynchronous mode

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
