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

	TRISC7 = 1;   // set RC7 as an input
	ANS9 = 1;     // set RC7 as analogue

	int reading_AN9 = 0;
	int reading_AN10 = 0;

	while (1)
	{
		reading_AN9 = adc_read(9);
		reading_AN10 = adc_read(10);
		serial_printf("AN9: %d, AN10: %d" NEWLINE, reading_AN9, reading_AN10);
		_delay(100000);
	}
}
