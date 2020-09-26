#include <xc.h>
#include <pic16f690.h>

#include <string.h>

#include "pins.h"
#include "utils.h"
#include "init.h"
#include "led.h"
#include "button.h"
#include "uart.h"
#include "adc.h"
#include "accel.h"
#include "flex.h"

#define MAX_MESSAGE_LENGTH 10

struct Hand
{
	int accel [3];
	int flex [5];
};

enum Type
{
	SAMPLE,
	DEBUG,
	CALIBRATE
};

struct Header
{
	enum Type type;
	int length;
};

char * MESSAGES [] = {"sample", "debug", "calibrate"};

void init(void);
int sample(struct Hand * hand);
void calibrate(struct Calibration * calibration);
int send(struct Header * header, void * data);
int receive(struct Header * header, void * data);

void main(void)
{
	init();

	struct Hand hand;
	char message [10];

	while (1)
	{
		uart_scan(message, sizeof(message));

		if (strcmp(message, MESSAGES[SAMPLE]) == 0)
		{
			sample(&hand);
			// uart_transmit((char *)&hand, sizeof(hand));
			uart_print("X: %d Y: %d Z: %d F1: %d F2: %d F3: %d F4: %d F5: %d" NEWLINE,
						hand.accel[X],
						hand.accel[Y],
						hand.accel[Z],
						hand.flex[F1],
						hand.flex[F2],
						hand.flex[F3],
						hand.flex[F4],
						hand.flex[F5]
						);
		}
		else if (strcmp(message, MESSAGES[DEBUG]) == 0)
		{
			uart_print("Dexterity is running" NEWLINE);
		}
		else if (strcmp(message, MESSAGES[CALIBRATE]) == 0)
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
	if (hand == null)
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
