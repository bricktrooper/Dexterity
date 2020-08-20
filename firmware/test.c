#include <xc.h>
#include <pic16f690.h>

#include "init.h"
#include "uart.h"
#include "serial.h"
#include "adc.h"
#include "accel.h"
#include "flex.h"

#define FLEX_TEST

int init_test(void);
int uart_test(void);
int serial_test(void);
int adc_test(void);
int accel_test(void);
int flex_test(void);

void main(void)
{
#if defined INIT_TEST
	init_test();
#elif defined UART_TEST
	uart_test();
#elif defined SERIAL_TEST
	serial_test();
#elif defined ADC_TEST
	adc_test();
#elif defined ACCEL_TEST
	accel_test();
#elif defined FLEX_TEST
	flex_test();
#endif
}

/* Test code for PIC16F690 startup */

int init_test(void)
{
	init_hardware();

	while (1)
	{
#ifdef PIC_TEST_BOARD
		PORTCbits.RC0 = RA5;
		PORTCbits.RC1 = RA4;
#endif
	}
}

/* Test code for UART on PIC16F690 */

int uart_test(void)
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
/* Test code for PIC16F690 serial I/O */

int serial_test(void)
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

/* Test code for PIC16F690 Analogue-to-Digital Converter */

int adc_test(void)
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

/* Test code for ADXL335 accelerometer */

int accel_test(void)
{
	init_hardware();
	uart_init();
	adc_init();
	accel_init();

	serial_println(NEWLINE NEWLINE "======= ACCEL Test =======" NEWLINE);

	int x = 0;
	int y = 0;
	int z = 0;

	while (1)
	{
		x = accel_x();
		y = accel_y();
		z = accel_z();

		serial_printf("X: %d, Y: %d, Z: %d" NEWLINE, x, y, z);
		_delay(100);
	}
}

/* Test code for flex sensors */

int flex_test(void)
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
