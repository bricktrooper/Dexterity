/* Test code for UART on PIC16F690 */

#include <xc.h>
#include <pic16f690.h>

#include "init.h"
#include "uart.h"

void main(void)
{
	init_hardware();
	uart_init();

	while (1)
	{
		uart_send("Hello World!\n\r");
		_delay(1000000);
	}
}
