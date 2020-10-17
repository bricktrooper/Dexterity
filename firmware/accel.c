#include <xc.h>
#include <pic16f690.h>

#include <stddef.h>
#include <stdbool.h>

#include "dexterity.h"
#include "pins.h"
#include "adc.h"

#include "accel.h"

#define ACCEL_DEFAULT_ZERO   100

static struct Analogue settings [3];
static int channels [] = {X_CHANNEL, Y_CHANNEL, Z_CHANNEL};
static bool scaling_enabled = true;

void accel_init(void)
{
	adc_set_vref(VREF_EXTERNAL);   // use external VREF for conversions (3.3V)

	// X Axis //

	X_TRIS = 1;         // set X-axis pin as an input
	X_ANSEL = 1;        // set X-axis pin as analogue
	accel_default(X);   // use default analogue settings

	// Y Axis //

	Y_TRIS = 1;         // set Y-axis pin as an input
	Y_ANSEL = 1;        // set Y-axis pin as analogue
	accel_default(Y);   // use default analogue settings

	// Z Axis //

	Z_TRIS = 1;         // set Z-axis pin as an input
	Z_ANSEL = 1;        // set Z-axis pin as analogue
	accel_default(Z);   // use default analogue settings
}

void accel_enable_scaling(bool enable)
{
	scaling_enabled = enable;
}

void accel_calibrate(enum Direction direction, S16 min, S16 max, S16 zero)
{
	settings[direction].min = min;
	settings[direction].max = max;
	settings[direction].zero = zero;
}

void accel_default(enum Direction direction)
{
	accel_calibrate(direction, ADC_MIN, ADC_MAX, ACCEL_DEFAULT_ZERO);
}

int accel_settings(enum Direction direction, struct Analogue * analogue)
{
	if (analogue == NULL)
	{
		return ERROR;
	}

	analogue->min = settings[direction].min;
	analogue->max = settings[direction].max;
	analogue->zero = settings[direction].zero;

	return SUCCESS;
}

S16 accel_scaled(enum Direction direction)
{
	// scale acceleration reading from 'min' <-> '1023' to -100 <-> +100 centered at 'zero'
	return scale(accel_raw(direction), ACCEL_SCALE_RANGE, settings[direction].min, settings[direction].max, settings[direction].zero);
}

S16 accel_raw(enum Direction direction)
{
	return adc_read(channels[direction]);
}

S16 accel_read(enum Direction direction)
{
	if (scaling_enabled)
	{
		return accel_scaled(direction);
	}
	else
	{
		return accel_raw(direction);
	}
}
