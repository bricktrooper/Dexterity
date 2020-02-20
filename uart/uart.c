// screen tty.usbserial-AL065BVB 9600
// (ctrl + a + k) ---> y

#include <xc.h>
#include <pic16f690.h>

#include "uart.h"

#define null       0

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

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

int uart_transmit_data(char * data, int length)
{
	if (data == null || length < 0)
	{
		return ERROR;
	}

	int byte_count = 0;

	for (int i = 0; i < length; i++)
	{
		uart_transmit_byte(data[i]);
		byte_count++;
	}

	return byte_count;
}

int uart_receive_data(char * buffer, int length)
{
	if (buffer == null || length < 0)
	{
		return ERROR;
	}

	int byte_count = 0;

	for (int i = 0; i < length; i++)
	{
		buffer[i] = uart_receive_byte();
		byte_count++;
	}

	return byte_count;
}
