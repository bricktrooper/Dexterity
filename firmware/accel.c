#include <xc.h>
#include <pic16f690.h>

#include <stddef.h>
#include <stdbool.h>

#include "dexterity.h"
#include "pins.h"
#include "adc.h"

#include "accel.h"

static AccelCalibration scaling_calibration;
static Mode scaling_mode = RAW;
static int channels [] = {X_CHANNEL, Y_CHANNEL, Z_CHANNEL};

void accel_init(void)
{
	adc_vref(VREF_EXTERNAL);   // use external VREF for conversions (3.3V)
	accel_reset();             // use default analogue calibration

	// X Axis //

	X_TRIS = 1;         // set X-axis pin as an input
	X_ANSEL = 1;        // set X-axis pin as analogue

	// Y Axis //

	Y_TRIS = 1;         // set Y-axis pin as an input
	Y_ANSEL = 1;        // set Y-axis pin as analogue

	// Z Axis //

	Z_TRIS = 1;         // set Z-axis pin as an input
	Z_ANSEL = 1;        // set Z-axis pin as analogue
}

int accel_mode(Mode mode)
{
	if (mode >= NUM_MODES)
	{
		return ERROR;
	}

	scaling_mode = mode;
	return SUCCESS;
}

int accel_calibrate(AccelCalibration * calibration)
{
	if (calibration == NULL)
	{
		return ERROR;
	}

	scaling_calibration.range = calibration->range;

	for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		scaling_calibration.params[direction][MIN] = calibration->params[direction][MIN];
		scaling_calibration.params[direction][MAX] = calibration->params[direction][MAX];
		scaling_calibration.params[direction][CENTRE] = calibration->params[direction][CENTRE];
	}

	return SUCCESS;
}

int accel_settings(AccelCalibration * calibration)
{
	if (calibration == NULL)
	{
		return ERROR;
	}

	calibration->range = scaling_calibration.range;

	for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		calibration->params[direction][MIN] = scaling_calibration.params[direction][MIN];
		calibration->params[direction][MAX] = scaling_calibration.params[direction][MAX];
		calibration->params[direction][CENTRE] = scaling_calibration.params[direction][CENTRE];
	}

	return SUCCESS;
}

void accel_reset(void)
{
	scaling_calibration.range = ACCEL_DEFAULT_RANGE;

	for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		scaling_calibration.params[direction][MIN] = ADC_MIN;
		scaling_calibration.params[direction][MAX] = ADC_MAX;
		scaling_calibration.params[direction][CENTRE] = ACCEL_DEFAULT_ZERO;
	}
}

S16 accel_scaled(Direction direction)
{
	// scale acceleration reading from 'min' <-> '1023' to -100 <-> +100 centered at 'centre'
	return scale(accel_raw(direction),
	             scaling_calibration.range,
	             scaling_calibration.params[direction][MIN],
	             scaling_calibration.params[direction][MAX],
	             scaling_calibration.params[direction][CENTRE]);
}

S16 accel_raw(Direction direction)
{
	return adc_read(channels[direction]);
}

S16 accel_read(Direction direction)
{
	if (scaling_mode == SCALED)
	{
		return accel_scaled(direction);
	}
	else
	{
		return accel_raw(direction);
	}
}
