#include <xc.h>
#include <pic16f690.h>

#include <string.h>
#include <stdbool.h>

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

static struct Hand hand;
static enum Message message;
static bool scale_readings = true;

void main(void)
{
	init();

	while (1)
	{
		message = uart_receive_message();

		if (message == MESSAGE_SAMPLE)
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
		else if (message == MESSAGE_RAW)
		{
			// use raw ADC readings
			scale_readings = false;
		}
		else if (message == MESSAGE_SCALED)
		{
			// scale ADC readings using the calibration
			scale_readings = true;
		}
		else if (message == MESSAGE_CALIBRATE)
		{
			// set calibration
		}
		else if (message == MESSAGE_SETTINGS)
		{
			// send calibration
		}
		else
		{
			// We don't really care about the receiving the other messages
			// just continue with the loop here
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

	// ACCELEROMETER //

	adc_set_vref(EXTERNAL);

	if (scale_readings)
	{
		hand->accel[X] = accel_scaled(X);
		hand->accel[Y] = accel_scaled(Y);
		hand->accel[Z] = accel_scaled(Z);
	}
	else
	{
		hand->accel[X] = accel_raw(X);
		hand->accel[Y] = accel_raw(Y);
		hand->accel[Z] = accel_raw(Z);
	}

	// FLEX SENSORS //

	adc_set_vref(INTERNAL);

	if (scale_readings)
	{
		hand->flex[F1] = flex_scaled(F1);
		hand->flex[F2] = flex_scaled(F2);
		hand->flex[F3] = flex_scaled(F3);
		hand->flex[F4] = flex_scaled(F4);
		hand->flex[F5] = flex_scaled(F5);
	}
	else
	{
		hand->flex[F1] = flex_raw(F1);
		hand->flex[F2] = flex_raw(F2);
		hand->flex[F3] = flex_raw(F3);
		hand->flex[F4] = flex_raw(F4);
		hand->flex[F5] = flex_raw(F5);
	}

	return SUCCESS;
}
