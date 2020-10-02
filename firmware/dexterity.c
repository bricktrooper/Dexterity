#include <xc.h>
#include <pic16f690.h>

#include <string.h>

#include "dexterity.h"
#include "pins.h"
#include "utils.h"
#include "init.h"
#include "led.h"
#include "button.h"
#include "uart.h"
#include "adc.h"
#include "accel.h"
#include "flex.h"

void init(void);
int sample(struct Hand * hand);
void calibrate(struct Calibration * calibration);
int send(void * data, int size);
int receive(void * data, int size);

void main(void)
{
	init();

	struct Hand hand;
	char message [10];

	while (1)
	{
		uart_scan(message, sizeof(message));

		if (strcmp(message, MESSAGE_SAMPLE) == 0)
		{
			sample(&hand);
			uart_transmit((char *)&hand, sizeof(hand));
			// uart_print("X: %d Y: %d Z: %d F1: %d F2: %d F3: %d F4: %d F5: %d" NEWLINE,
			// 			hand.accel[X],
			// 			hand.accel[Y],
			// 			hand.accel[Z],
			// 			hand.flex[F1],
			// 			hand.flex[F2],
			// 			hand.flex[F3],
			// 			hand.flex[F4],
			// 			hand.flex[F5]
			// 			);
		}
		else if (strcmp(message, MESSAGE_CALIBRATE) == 0)
		{
			// calibrate here
		}
		else
		{
			// unknown packet
		}
	}
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

int sample(struct Hand * hand)
{
	if (hand == NULL)
	{
		return ERROR;
	}

	adc_set_vref(EXTERNAL);
	hand->accel[X] = accel_raw(X);
	hand->accel[Y] = accel_raw(Y);
	hand->accel[Z] = accel_raw(Z);

	adc_set_vref(INTERNAL);
	hand->flex[F1] = flex_raw(F1);
	hand->flex[F2] = flex_raw(F2);
	hand->flex[F3] = flex_raw(F3);
	hand->flex[F4] = flex_raw(F4);
	hand->flex[F5] = flex_raw(F5);

	return SUCCESS;
}
