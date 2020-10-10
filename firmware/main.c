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
int calibrate(struct Calibration * calibration);
int send(void * data, int size);
int receive(void * data, int size);

void main(void)
{
	init();

	struct Hand hand;
	struct Calibration calibration;
	enum Message message;

	while (1)
	{
		message = uart_receive_message();

		if (message == MESSAGE_SAMPLE)
		{
			sample(&hand);
			uart_transmit((char *)&hand, sizeof(hand));
			// uart_print("X: %d Y: %d Z: %d F1: %d F2: %d F3: %d F4: %d F5: %d BUTTON: %d LED: %d" NEWLINE,
			// 			hand.accel[X],
			// 			hand.accel[Y],
			// 			hand.accel[Z],
			// 			hand.flex[F1],
			// 			hand.flex[F2],
			// 			hand.flex[F3],
			// 			hand.flex[F4],
			// 			hand.flex[F5],
			// 			hand.button,
			// 			hand.led
			// 			);
		}
		else if (message == MESSAGE_RAW)
		{
			// use raw ADC readings
			accel_enable_scaling(false);
			flex_enable_scaling(false);
		}
		else if (message == MESSAGE_SCALED)
		{
			// scale ADC readings using the calibration
			accel_enable_scaling(true);
			flex_enable_scaling(true);
		}
		else if (message == MESSAGE_CALIBRATE)
		{
			int size = sizeof(struct Calibration);

			if (uart_receive((char *)&calibration, size) != size)
			{
				// TODO TODO TODO should probably send an back error message here as well!!!!!!!!
				continue;
			}
			else
			{
				calibrate(&calibration);
			}
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

	hand->accel[X] = accel_read(X);
	hand->accel[Y] = accel_read(Y);
	hand->accel[Z] = accel_read(Z);

	// FLEX SENSORS //

	adc_set_vref(INTERNAL);

	hand->flex[F1] = flex_read(F1);
	hand->flex[F2] = flex_read(F2);
	hand->flex[F3] = flex_read(F3);
	hand->flex[F4] = flex_read(F4);
	hand->flex[F5] = flex_read(F5);

	// BUTTON STATE //

	hand->button = BUTTON;

	// LED STATE //

	hand->led = LED;

	return SUCCESS;
}

int calibrate(struct Calibration * calibration)
{
	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		accel_calibrate(direction, calibration->accel[direction].min, calibration->accel[direction].max, calibration->accel[direction].zero);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		flex_calibrate(finger, calibration->flex[finger].min, calibration->flex[finger].max, calibration->flex[finger].zero);
	}

	return SUCCESS;
}
