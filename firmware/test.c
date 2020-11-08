#include <xc.h>
#include <pic16f690.h>

#include "dexterity.h"
#include "utils.h"
#include "pins.h"
#include "init.h"
#include "uart.h"
#include "adc.h"
#include "accel.h"
#include "flex.h"
#include "led.h"
#include "button.h"

void init_test(void);
void uart_test(void);
void adc_test(void);
void accel_test(void);
void flex_test(void);
void led_test(void);
void button_test(void);

void main(void)
{
#if defined INIT_TEST
	init_test();
#elif defined UART_TEST
	uart_test();
#elif defined ADC_TEST
	adc_test();
#elif defined ACCEL_TEST
	accel_test();
#elif defined FLEX_TEST
	flex_test();
#elif defined LED_TEST
	led_test();
#elif defined BUTTON_TEST
	button_test();
#else
	init_hardware();
	while (1);
#endif
}

/* Test code for PIC16F690 startup */

void init_test(void)
{
	init_hardware();

	while (1)
	{
#ifdef PIC_TEST_BOARD
		RC0 = RA5;
		RC1 = RA4;
#else
		LED = 1;
		_delay(1000000);
		LED = 0;
		_delay(1000000);
#endif
	}
}

/* Test code for UART on PIC16F690 */

void uart_test(void)
{
	init_hardware();
	uart_init();

	uart_print(NEWLINE NEWLINE "======= UART Test =======" NEWLINE NEWLINE);

	uart_print("Hello" NEWLINE);
	uart_print("number = %d" NEWLINE, 7);

	uart_print("Please enter a message: ");
	enum Message message = uart_receive_message();
	uart_print(NEWLINE);
	uart_print("Your message was: ");
	uart_transmit_message(message);
	uart_print(NEWLINE);

	char input [50];
	uart_print("Type up to %d characters and watch them echo back:" NEWLINE, sizeof(input));

	while (1)
	{
		uart_receive(input, 50);
		uart_transmit(input, 50);
	}
}

/* Test code for PIC16F690 Analogue-to-Digital Converter */

void adc_test(void)
{
	init_hardware();
	uart_init();
	adc_init();

	uart_print(NEWLINE NEWLINE "======= ADC Test =======" NEWLINE NEWLINE);

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
		uart_print("AN9: %d, AN10: %d" NEWLINE, reading_AN9, reading_AN10);
		_delay(100000);
	}
}

/* Test code for ADXL335 accelerometer */

void accel_test(void)
{
	init_hardware();
	uart_init();
	adc_init();
	accel_init();

	uart_print(NEWLINE NEWLINE "======= ACCEL Test =======" NEWLINE NEWLINE);

	int x = 0;
	int y = 0;
	int z = 0;

	while (1)
	{
		x = accel_read(X);
		y = accel_read(Y);
		z = accel_read(Z);

		uart_print("X: %d, Y: %d, Z: %d" NEWLINE, x, y, z);
		_delay(100);
	}
}

/* Test code for flex sensors */

void flex_test(void)
{
	init_hardware();
	uart_init();
	adc_init();
	flex_init();

	uart_print(NEWLINE NEWLINE "======= FLEX Test =======" NEWLINE NEWLINE);

	int thumb = 0;
	int index = 0;
	int middle = 0;
	int ring = 0;
	int pinky = 0;

	while (1)
	{
		thumb = flex_read(THUMB);
		index = flex_read(INDEX);
		middle = flex_read(MIDDLE);
		ring = flex_read(RING);
		pinky = flex_read(PINKY);

		uart_print("1: %d, 2: %d, 3: %d, 4: %d, 5: %d" NEWLINE, thumb, index, middle, ring, pinky);
		_delay(10);
	}
}

void led_test(void)
{
	init_hardware();
	led_init();

	while (1)
	{
		led_blink(3, 100);
		delay_ms(1000);
	}
}

void button_test(void)
{
	init_hardware();
	led_init();
	button_init();

	led_off();

	while (1)
	{
		button_wait_for_click();
		led_blink(1, 100);
	}
}
