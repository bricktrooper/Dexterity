#include <xc.h>
#include <pic16f690.h>

#include "pins.h"
#include "led.h"

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

int delay_ms(long duration)
{
	if (duration < 0)
	{
		return ERROR;
	}

	for (long i = 0; i < duration; i++)
	{
		_delay(1000);   // delay for 1ms
	}

	return SUCCESS;
}

void led_init(void)
{
	LED_TRIS = 0;
}

void led_on(void)
{
	LED = 1;
}

void led_off(void)
{
	LED = 0;
}

int led_blink(int blinks, long duration)
{
	// blinks: number of times to blink LED
	// duration: time of blink in milliseconds

	if (blinks < 1 || duration < 0)
	{
		return ERROR;
	}

	for (int i = 0; i < blinks; i++)
	{
		led_on();
		delay_ms(duration);
		led_off();
		delay_ms(duration);
	}

	return SUCCESS;
}
