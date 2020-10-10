#include <xc.h>
#include <pic16f690.h>

#include <stdbool.h>

#include "dexterity.h"
#include "pins.h"
#include "adc.h"

#include "flex.h"

#define FLEX_DEFAULT_ZERO   0

static struct Analogue settings [5];
static int channels [] = {F1_CHANNEL, F2_CHANNEL, F3_CHANNEL, F4_CHANNEL, F5_CHANNEL};
static bool scaling_enabled = true;

void flex_init(void)
{
	adc_set_vref(INTERNAL);   // use VDD as VREF for conversions (5V)

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

S16 flex_scale(S16 reading, S16 min, S16 max, S16 zero)
{
	if (max == min)  // avoid divide by 0
	{
		return ERROR;
	}

	// scale flex reading from 'min' <-> 'max' to 0 <-> 100 centred at 'zero'
	return ((100 * (reading - min)) / (max - min)) - zero;
}

void flex_enable_scaling(bool enable)
{
	scaling_enabled = enable;
}

void flex_calibrate(enum Finger finger, S16 min, S16 max, S16 zero)
{
	settings[finger].min = min;
	settings[finger].max = max;
	settings[finger].zero = zero;
}

void flex_default(enum Finger finger)
{
	flex_calibrate(finger, ADC_MIN, ADC_MAX, FLEX_DEFAULT_ZERO);
}

S16 flex_scaled(enum Finger finger)
{
	return flex_scale(flex_raw(finger), settings[finger].min, settings[finger].max, settings[finger].zero);
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
