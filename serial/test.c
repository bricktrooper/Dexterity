/* Test code for PIC16F690 serial I/O */

#include <xc.h>
#include <pic16f690.h>

#include "init.h"
#include "uart.h"
#include "serial.h"

void main(void)
{
	init_hardware();
	uart_init();

	serial_println("");
	serial_print("serial_print(): Hello\r\n");
	serial_println("serial_println(): Hello");
	serial_printf("serial_printf(): number = %d\r\n", 7);
	serial_print("serial_printc(): ");
	serial_printc('K');
	serial_println("");

	while (1);
}
