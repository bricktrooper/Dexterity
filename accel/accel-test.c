/* Test code for ADXL335 accelerometer */

#include <xc.h>
#include <pic16f690.h>

#include "init.h"
#include "uart.h"
#include "serial.h"
#include "adc.h"
#include "accel.h"

void main(void)
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
