#include <xc.h>
#include <pic16f690.h>

#include "uart.h"

void uart_init(void)
{
	// TRANSMITTER (TX) //

	// TXEN = 1;   // enable TX circuitry
	// SYNC = 0;   // use asynchronous mode
	// SPEN = 1;   // enable serial port pins
	// TX9 = 0;    // use 8-bit transmission

	// BAUD RATE (9600 bps, 4MHz Fosc) //

	// BRGH = 0;   // use lo baud rate
	// BRG16 = 0;  // use 8-bit baud rate generator

	// SPBRG = 25; // lower register for BRG setting
	// SPBRGH = 0; // higher register for BRG setting

	TXSTAbits.TX9 = 0;
	TXSTAbits.TXEN = 1;
	TXSTAbits.SYNC = 0;
	TXSTAbits.BRGH = 1;

	RCSTAbits.SPEN = 1;

	BAUDCTLbits.BRG16 = 0;
	BAUDCTLbits.SCKP = 0;

	SPBRG = 23;
	SPBRGH = 0;
}
