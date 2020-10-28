#include <xc.h>
#include <pic16f690.h>

#include <stddef.h>
#include <stdbool.h>

#include "dexterity.h"
#include "pins.h"
#include "adc.h"

#include "accel.h"

#define ACCEL_DEFAULT_ZERO   100

static struct Accel settings;
static int channels [] = {X_CHANNEL, Y_CHANNEL, Z_CHANNEL};
static bool scaling_enabled = true;

void accel_init(void)
{
	adc_set_vref(VREF_EXTERNAL);                  // use external VREF for conversions (3.3V)
	accel_set_range(ACCEL_DEFAULT_SCALE_RANGE);   // use default scaling range

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

S16 accel_get_range(void)
{
	return settings.range;
}

void accel_set_range(S16 range)
{
	settings.range = range;
}

void accel_calibrate(enum Direction direction, S16 min, S16 max, S16 zero)
{
	settings.params[direction].min = min;
	settings.params[direction].max = max;
	settings.params[direction].zero = zero;
}

void accel_default(enum Direction direction)
{
	accel_calibrate(direction, ADC_MIN, ADC_MAX, ACCEL_DEFAULT_ZERO);
}

int accel_settings(enum Direction direction, struct Parameters * param)
{
	if (param == NULL)
	{
		return ERROR;
	}

	param->min = settings.params[direction].min;
	param->max = settings.params[direction].max;
	param->zero = settings.params[direction].zero;

	return SUCCESS;
}

S16 accel_scaled(enum Direction direction)
{
	// scale acceleration reading from 'min' <-> '1023' to -100 <-> +100 centered at 'zero'
	return scale(accel_raw(direction), settings.range, settings.params[direction].min, settings.params[direction].max, settings.params[direction].zero);
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
