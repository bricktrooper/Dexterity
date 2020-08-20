/* Test code for flex sensors */

#include <xc.h>
#include <pic16f690.h>

#include "init.h"
#include "uart.h"
#include "serial.h"
#include "adc.h"
#include "flex.h"

void main(void)
{
	init_hardware();
	uart_init();
	adc_init();
	flex_init();

	serial_println(NEWLINE NEWLINE "======= FLEX Test =======" NEWLINE);

	int f1 = 0;
	int f2 = 0;
	int f3 = 0;
	int f4 = 0;
	int f5 = 0;

	while (1)
	{
		f1 = flex_f1();
		f2 = flex_f2();
		f3 = flex_f3();
		f4 = flex_f4();
		f5 = flex_f5();

		serial_printf("1: %d, 2: %d, 3: %d, 4: %d, 5: %d" NEWLINE, f1, f2, f3, f4, f5);
		_delay(10);
	}
}
