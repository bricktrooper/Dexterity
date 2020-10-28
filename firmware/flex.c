#include <xc.h>
#include <pic16f690.h>

#include <stddef.h>
#include <stdbool.h>

#include "dexterity.h"
#include "pins.h"
#include "adc.h"

#include "flex.h"

#define FLEX_DEFAULT_ZERO   0

static struct Flex settings;
static int channels [] = {F1_CHANNEL, F2_CHANNEL, F3_CHANNEL, F4_CHANNEL, F5_CHANNEL};
static bool scaling_enabled = true;

void flex_init(void)
{
	adc_set_vref(VREF_INTERNAL);                // use VDD as VREF for conversions (5V)
	flex_set_range(FLEX_DEFAULT_SCALE_RANGE);   // use default scaling range

	// FINGER 1 //

	F1_TRIS = 1;        // set finger 1 pin as an input
	F1_ANSEL = 1;       // set finger 1 pin as analogue
	flex_default(F1);   // use default analogue settings

	// FINGER 2 //

	F2_TRIS = 1;        // set finger 2 pin as an input
	F2_ANSEL = 1;       // set finger 2 pin as analogue
	flex_default(F2);   // use default analogue settings

	// FINGER 1 //

	F3_TRIS = 1;        // set finger 3 pin as an input
	F3_ANSEL = 1;       // set finger 3 pin as analogue
	flex_default(F3);   // use default analogue settings

	// FINGER 4 //

	F4_TRIS = 1;        // set finger 4 pin as an input
	F4_ANSEL = 1;       // set finger 4 pin as analogue
	flex_default(F4);   // use default analogue settings

	// FINGER 5 //

	F5_TRIS = 1;        // set finger 5 pin as an input
	F5_ANSEL = 1;       // set finger 5 pin as analogue
	flex_default(F5);   // use default analogue settings
}

void flex_enable_scaling(bool enable)
{
	scaling_enabled = enable;
}

S16 flex_get_range(void)
{
	return settings.range;
}

void flex_set_range(S16 range)
{
	settings.range = range;
}

void flex_calibrate(enum Finger finger, S16 min, S16 max, S16 zero)
{
	settings.params[finger].min = min;
	settings.params[finger].max = max;
	settings.params[finger].zero = zero;
}

void flex_default(enum Finger finger)
{
	flex_calibrate(finger, ADC_MIN, ADC_MAX, FLEX_DEFAULT_ZERO);
}

int flex_settings(enum Finger finger, struct Parameters * param)
{
	if (param == NULL)
	{
		return ERROR;
	}

	param->min = settings.params[finger].min;
	param->max = settings.params[finger].max;
	param->zero = settings.params[finger].zero;

	return SUCCESS;
}

S16 flex_scaled(enum Finger finger)
{
	// scale flex reading from 'min' <-> 'max' to 0 <-> 100 centred at 'zero'
	return scale(flex_raw(finger), settings.range, settings.params[finger].min, settings.params[finger].max, settings.params[finger].zero);
}

S16 flex_raw(enum Finger finger)
{
	return adc_read(channels[finger]);
}

S16 flex_read(enum Finger finger)
{
	if (scaling_enabled)
	{
		return flex_scaled(finger);
	}
	else
	{
		return flex_raw(finger);
	}
}
