/* Test code for UART on PIC16F690 */

#include <xc.h>
#include <pic16f690.h>

#include "init.h"
#include "uart.h"

void main(void)
{
	init_hardware();
	uart_init();
	_delay(1000000);

	while (1)
	{
		PORTC = OSCCON;
		while (TXSTAbits.TRMT == 0);    // Wait for buffer to be empty
		TXREG = 'u';
	}
}
