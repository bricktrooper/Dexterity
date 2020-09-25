#include <xc.h>
#include <pic16f690.h>

#include "pins.h"
#include "utils.h"
#include "init.h"
#include "led.h"
#include "button.h"
#include "uart.h"
#include "adc.h"
#include "accel.h"
#include "flex.h"

struct Readings
{
	int accel [3];
	int flex [5];
};

enum PacketType
{
	SAMPLE,
	DEBUG,
	CALIBRATE
};

struct PacketHeader
{
	enum PacketType type;
	int length;
};

void init(void);
int sample(struct Readings * readings);
void calibrate(void);
int send(struct PacketHeader * header, char * data);
int receive(struct PacketHeader * header, char * data);

void main(void)
{
	init();
	struct Readings readings;

	while (1)
	{
		sample(&readings);
		uart_print("X: %d Y: %d Z: %d F1: %d F2: %d F3: %d F4: %d F5: %d" NEWLINE,
					readings.accel[X],
					readings.accel[Y],
					readings.accel[Z],
					readings.flex[F1],
					readings.flex[F2],
					readings.flex[F3],
					readings.flex[F4],
					readings.flex[F5]
					);
	}
}

int sample(struct Readings * readings)
{
	if (readings == null)
	{
		return ERROR;
	}

	adc_set_vref(EXTERNAL);
	readings->accel[X] = accel_read(X);
	readings->accel[Y] = accel_read(Y);
	readings->accel[Z] = accel_read(Z);

	adc_set_vref(INTERNAL);
	readings->flex[F1] = flex_read(F1);
	readings->flex[F2] = flex_read(F2);
	readings->flex[F3] = flex_read(F3);
	readings->flex[F4] = flex_read(F4);
	readings->flex[F5] = flex_read(F5);

	return SUCCESS;
}

void init(void)
{
	init_hardware();
	led_init();
	button_init();
	uart_init();
	adc_init();
	accel_init();
	flex_init();
}
