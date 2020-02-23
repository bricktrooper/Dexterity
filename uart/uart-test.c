/* Test code for UART on PIC16F690 */

#include <xc.h>
#include <pic16f690.h>

#include "init.h"
#include "uart.h"

void main(void)
{
	init_hardware();
	uart_init();

	char buffer [50];

	while (1)
	{
		uart_receive_data(buffer, 50);
		uart_transmit_data(buffer, 50);
	}
}
