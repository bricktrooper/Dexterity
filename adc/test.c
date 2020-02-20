/* Test code for PIC16F690 Analogue-to-Digital Converter */

#include <xc.h>
#include <pic16f690.h>

#include "init.h"
#include "uart.h"
#include "serial.h"
#include "adc.h"

void main(void)
{
	init_hardware();
	uart_init();
	adc_init();

	serial_println(NEWLINE NEWLINE "======= ADC Test =======" NEWLINE);

	TRISB4 = 1;   // set RB4 as an input
	ANS10 = 1;    // set RB4 as analogue

	int reading = 0;

	while (1)
	{
		reading = adc_read(10);
		serial_printf("AN10 Reading: %u" NEWLINE, reading);
		PORTC = reading;
		_delay(100000);
	}
}
