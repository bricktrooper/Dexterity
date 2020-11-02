#include <xc.h>
#include <pic16f690.h>

#include <stddef.h>
#include <stdbool.h>

#include "dexterity.h"
#include "pins.h"
#include "adc.h"

#include "flex.h"

static struct FlexCalibration scaling_calibration;
static enum Mode scaling_mode = RAW;
static int channels [] = {F1_CHANNEL, F2_CHANNEL, F3_CHANNEL, F4_CHANNEL, F5_CHANNEL};

void flex_init(void)
{
	adc_vref(VREF_INTERNAL);   // use VDD as VREF for conversions (5V)
	flex_reset();              // use default analogue calibration

	// FINGER 1 //

	F1_TRIS = 1;        // set finger 1 pin as an input
	F1_ANSEL = 1;       // set finger 1 pin as analogue

	// FINGER 2 //

	F2_TRIS = 1;        // set finger 2 pin as an input
	F2_ANSEL = 1;       // set finger 2 pin as analogue

	// FINGER 1 //

	F3_TRIS = 1;        // set finger 3 pin as an input
	F3_ANSEL = 1;       // set finger 3 pin as analogue

	// FINGER 4 //

	F4_TRIS = 1;        // set finger 4 pin as an input
	F4_ANSEL = 1;       // set finger 4 pin as analogue

	// FINGER 5 //

	F5_TRIS = 1;        // set finger 5 pin as an input
	F5_ANSEL = 1;       // set finger 5 pin as analogue
}

int flex_mode(enum Mode mode)
{
	if (mode >= NUM_MODES)
	{
		return ERROR;
	}

	scaling_mode = mode;
	return SUCCESS;
}

int flex_calibrate(struct FlexCalibration * calibration)
{
	if (calibration == NULL)
	{
		return ERROR;
	}

	scaling_calibration.range = calibration->range;

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		scaling_calibration.params[finger][MIN] = calibration->params[finger][MIN];
		scaling_calibration.params[finger][MAX] = calibration->params[finger][MAX];
		scaling_calibration.params[finger][CENTRE] = calibration->params[finger][CENTRE];
	}

	return SUCCESS;
}

int flex_settings(struct FlexCalibration * calibration)
{
	if (calibration == NULL)
	{
		return ERROR;
	}

	calibration->range = scaling_calibration.range;

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		calibration->params[finger][MIN] = scaling_calibration.params[finger][MIN];
		calibration->params[finger][MAX] = scaling_calibration.params[finger][MAX];
		calibration->params[finger][CENTRE] = scaling_calibration.params[finger][CENTRE];
	}

	return SUCCESS;
}

void flex_reset(void)
{
	scaling_calibration.range = FLEX_DEFAULT_RANGE;

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		scaling_calibration.params[finger][MIN] = ADC_MIN;
		scaling_calibration.params[finger][MAX] = ADC_MAX;
		scaling_calibration.params[finger][CENTRE] = FLEX_DEFAULT_ZERO;
	}
}

S16 flex_scaled(enum Finger finger)
{
	// scale flex reading from 'min' <-> 'max' to 0 <-> 100 centred at 'centre'
	return scale(flex_raw(finger),
	             scaling_calibration.range,
	             scaling_calibration.params[finger][MIN],
	             scaling_calibration.params[finger][MAX],
	             scaling_calibration.params[finger][CENTRE]);
}

S16 flex_raw(enum Finger finger)
{
	return adc_read(channels[finger]);
}

S16 flex_read(enum Finger finger)
{
	if (scaling_mode == SCALED)
	{
		return flex_scaled(finger);
	}
	else
	{
		return flex_raw(finger);
	}
}
