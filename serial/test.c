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

	serial_println(NEWLINE NEWLINE "======= SERIAL Test =======" NEWLINE);

	serial_print("serial_print(): Hello" NEWLINE);
	serial_println("serial_println(): Hello");
	serial_printf("serial_printf(): number = %d" NEWLINE, 7);
	serial_print("serial_printc(): ");
	serial_printc('K');
	serial_print(NEWLINE);

	char input [50];
	serial_print("Please enter some text: ");
	serial_printf(NEWLINE "bytes read: %d" NEWLINE, serial_read(input, 50));
	serial_printf("You entered: %s" NEWLINE, input);

	while (1);
}
