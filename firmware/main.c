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
int settings(struct Calibration * calibration);

void main(void)
{
	init();

	struct Hand hand;
	struct Calibration calibration;
	enum Message message;

	while (1)
	{
		// operations that change the device state will require an ACK
		// operations that simply request data do not require a separate ACK
		message = uart_receive_message();

		switch (message)
		{
			case MESSAGE_SAMPLE:   // read the analogue sensors and transmit the results

				sample(&hand);
				uart_transmit(&hand, sizeof(struct Hand));
				break;

			case MESSAGE_RAW:   // use raw ADC readings

				accel_enable_scaling(false);
				flex_enable_scaling(false);
				uart_transmit_message(MESSAGE_SUCCESS);
				break;

			case MESSAGE_SCALED:   // scale ADC readings using the calibration

				accel_enable_scaling(true);
				flex_enable_scaling(true);
				uart_transmit_message(MESSAGE_SUCCESS);
				break;

			case MESSAGE_UPLOAD:   // receive the calibration settings and apply them

				uart_transmit_message(MESSAGE_SUCCESS);   // handshake with PC

				if (uart_receive(&calibration, sizeof(struct Calibration)) != sizeof(struct Calibration))
				{
					uart_transmit_message(MESSAGE_ERROR);
					break;
				}

				// apply the calibration and send an ACK when finished
				calibrate(&calibration);
				uart_transmit_message(MESSAGE_SUCCESS);
				break;

			case MESSAGE_DOWNLOAD:   // transmit the current calibration settings

				settings(&calibration);
				uart_transmit(&calibration, sizeof(struct Calibration));
				break;

			default:
				// We don't really care about receiving the other messages
				// just continue with the state machine here
				break;

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

	adc_set_vref(VREF_EXTERNAL);

	hand->accel[X] = accel_read(X);
	hand->accel[Y] = accel_read(Y);
	hand->accel[Z] = accel_read(Z);

	// FLEX SENSORS //

	adc_set_vref(VREF_INTERNAL);

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
	if (calibration == NULL)
	{
		return ERROR;
	}

	accel_set_range(calibration->accel.range);

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		accel_calibrate(direction, calibration->accel.params[direction].min, calibration->accel.params[direction].max, calibration->accel.params[direction].zero);
	}

	flex_set_range(calibration->flex.range);

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		flex_calibrate(finger, calibration->flex.params[finger].min, calibration->flex.params[finger].max, calibration->flex.params[finger].zero);
	}

	return SUCCESS;
}

int settings(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		return ERROR;
	}

	calibration->accel.range = accel_get_range();

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		accel_settings(direction, &(calibration->accel.params[direction]));
	}

	calibration->flex.range = flex_get_range();

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		flex_settings(finger, &(calibration->flex.params[finger]));
	}

	return SUCCESS;
}
