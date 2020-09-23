#include <xc.h>
#include <pic16f690.h>

#include "utils.h"
#include "pins.h"

#include "led.h"

void led_init(void)
{
	LED_TRIS = 0;   // set LED as output
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
