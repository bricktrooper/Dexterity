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
void raw(void);
void scaled(void);
int calibrate(struct Calibration * calibration);
int settings(struct Calibration * calibration);

void main(void)
{
	init();
	led_off();

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

				raw();
				uart_transmit_message(MESSAGE_SUCCESS);
				break;

			case MESSAGE_SCALED:   // scale ADC readings using the calibration

				scaled();
				uart_transmit_message(MESSAGE_SUCCESS);
				break;

			case MESSAGE_UPLOAD:   // receive the calibration settings and apply them

				uart_transmit_message(MESSAGE_SUCCESS);   // handshake with the host

				if (uart_receive(&calibration, sizeof(struct Calibration)) != sizeof(struct Calibration))
				{
					uart_transmit_message(MESSAGE_ERROR);
					break;
				}

				calibrate(&calibration);                  // apply the calibration
				uart_transmit_message(MESSAGE_SUCCESS);   // send an ACK when finished
				break;

			case MESSAGE_DOWNLOAD:   // transmit the current calibration settings

				settings(&calibration);                                    // copy the current calibration settings
				uart_transmit(&calibration, sizeof(struct Calibration));   // send the calibration back to the host
				break;

			case MESSAGE_UNKNOWN:   // respond with error if an unexpected or unknown message is received
			default:

				uart_transmit_message(MESSAGE_ERROR);
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

	adc_vref(VREF_EXTERNAL);

	hand->accel[X] = accel_read(X);
	hand->accel[Y] = accel_read(Y);
	hand->accel[Z] = accel_read(Z);

	// FLEX SENSORS //

	adc_vref(VREF_INTERNAL);

	hand->flex[THUMB] = flex_read(THUMB);
	hand->flex[INDEX] = flex_read(INDEX);
	hand->flex[MIDDLE] = flex_read(MIDDLE);
	hand->flex[RING] = flex_read(RING);
	hand->flex[PINKY] = flex_read(PINKY);

	// BUTTON STATE //

	hand->button = BUTTON;

	// LED STATE //

	hand->led = LED;

	return SUCCESS;
}

void raw(void)
{
	accel_mode(RAW);
	flex_mode(RAW);
}

void scaled(void)
{
	accel_mode(SCALED);
	flex_mode(SCALED);
}


int calibrate(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		return ERROR;
	}

	if (accel_calibrate(&calibration->accel) == ERROR ||
		flex_calibrate(&calibration->flex) == ERROR)
	{
		return ERROR;
	}

	return SUCCESS;
}

int settings(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		return ERROR;
	}

	if (accel_settings(&calibration->accel) == ERROR ||
		flex_settings(&calibration->flex) == ERROR)
	{
		return ERROR;
	}

	return SUCCESS;
}
