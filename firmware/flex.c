#include <xc.h>
#include <pic16f690.h>

#include "dexterity.h"
#include "pins.h"
#include "adc.h"

#include "flex.h"

#define FLEX_DEFAULT_ZERO   0

static struct Calibration calibration [5];
static int channels [] = {F1_CHANNEL, F2_CHANNEL, F3_CHANNEL, F4_CHANNEL, F5_CHANNEL};

void flex_init(void)
{
	adc_set_vref(INTERNAL);   // use VDD as VREF for conversions (5V)

	// FINGER 1 //

	F1_TRIS = 1;        // set finger 1 pin as an input
	F1_ANSEL = 1;       // set finger 1 pin as analogue
	flex_default(F1);   // use default calibration

	// FINGER 2 //

	F2_TRIS = 1;        // set finger 2 pin as an input
	F2_ANSEL = 1;       // set finger 2 pin as analogue
	flex_default(F2);   // use default calibration

	// FINGER 1 //

	F3_TRIS = 1;        // set finger 3 pin as an input
	F3_ANSEL = 1;       // set finger 3 pin as analogue
	flex_default(F3);   // use default calibration

	// FINGER 4 //

	F4_TRIS = 1;        // set finger 4 pin as an input
	F4_ANSEL = 1;       // set finger 4 pin as analogue
	flex_default(F4);   // use default calibration

	// FINGER 5 //

	F5_TRIS = 1;        // set finger 5 pin as an input
	F5_ANSEL = 1;       // set finger 5 pin as analogue
	flex_default(F5);   // use default calibration
}

S16 flex_scale(S16 reading, S16 min, S16 max, S16 zero)
{
	if (max - min == 0)  // avoid divide by 0
	{
		return ERROR;
	}

	// scale flex reading from 'min' <-> 'max' to 0 <-> 100 centred at 'zero'
	return ((100 * (reading - min)) / (max - min)) - zero;
}

void flex_calibrate(enum Finger finger, S16 min, S16 max, S16 zero)
{
	calibration[finger].min = min;
	calibration[finger].max = max;
	calibration[finger].zero = zero;
}

void flex_default(enum Finger finger)
{
	flex_calibrate(finger, ADC_MIN, ADC_MAX, FLEX_DEFAULT_ZERO);
}

S16 flex_scaled(enum Finger finger)
{
	return flex_scale(flex_raw(finger), calibration[finger].min, calibration[finger].max, calibration[finger].zero);
}

S16 flex_raw(enum Finger finger)
{
	return adc_read(channels[finger]);
}
