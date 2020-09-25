#include <xc.h>
#include <pic16f690.h>

#include "utils.h"
#include "pins.h"
#include "adc.h"

#include "flex.h"

#define FLEX_DEFAULT_ZERO   0

static struct Analogue sensors [5];

void flex_init(void)
{
	adc_set_vref(INTERNAL);   // use VDD as VREF for conversions (5V)

	// FINGER 1 //

	F1_TRIS = 1;      // set finger 1 pin as an input
	F1_ANSEL = 1;     // set finger 1 pin as analogue

	sensors[F1].channel = F1_CHANNEL;
	flex_default(F1);

	// FINGER 2 //

	F2_TRIS = 1;      // set finger 2 pin as an input
	F2_ANSEL = 1;     // set finger 2 pin as analogue

	sensors[F2].channel = F2_CHANNEL;
	flex_default(F2);

	// FINGER 1 //

	F3_TRIS = 1;      // set finger 3 pin as an input
	F3_ANSEL = 1;     // set finger 3 pin as analogue

	sensors[F3].channel = F3_CHANNEL;
	flex_default(F3);

	// FINGER 4 //

	F4_TRIS = 1;      // set finger 4 pin as an input
	F4_ANSEL = 1;     // set finger 4 pin as analogue

	sensors[F4].channel = F4_CHANNEL;
	flex_default(F4);

	// FINGER 5 //

	F5_TRIS = 1;      // set finger 5 pin as an input
	F5_ANSEL = 1;     // set finger 5 pin as analogue

	sensors[F5].channel = F5_CHANNEL;
	flex_default(F5);
}

int flex_scale(int reading, int min, int max, int zero)
{
	if (max - min == 0)  // avoid divide by 0
	{
		return ERROR;
	}

	// scale flex reading from 'min' <-> 'max' to 0 <-> 100 centred at 'zero'
	return ((100 * (reading - min)) / (max - min)) - zero;
}

void flex_calibrate(enum Finger finger, int min, int max, int zero)
{
	sensors[finger].min = min;
	sensors[finger].max = max;
	sensors[finger].zero = zero;
}

void flex_default(enum Finger finger)
{
	flex_calibrate(finger, ADC_MIN, ADC_MAX, FLEX_DEFAULT_ZERO);
}

int flex_read(enum Finger finger)
{
	return flex_scale(flex_raw(finger), sensors[finger].min, sensors[finger].max, sensors[finger].zero);
}

int flex_raw(enum Finger finger)
{
	return adc_read(sensors[finger].channel);
}
