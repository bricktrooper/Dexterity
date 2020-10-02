#include <xc.h>
#include <pic16f690.h>

#include "dexterity.h"
#include "pins.h"
#include "adc.h"

#include "accel.h"

#define ACCEL_DEFAULT_ZERO   100

static struct Calibration calibration [3];
static int channels [] = {X_CHANNEL, Y_CHANNEL, Z_CHANNEL};

void accel_init(void)
{
	adc_set_vref(EXTERNAL);   // use external VREF for conversions (3.3V)

	// X Axis //

	X_TRIS = 1;         // set X-axis pin as an input
	X_ANSEL = 1;        // set X-axis pin as analogue
	accel_default(X);   // use default calibration

	// Y Axis //

	Y_TRIS = 1;         // set Y-axis pin as an input
	Y_ANSEL = 1;        // set Y-axis pin as analogue
	accel_default(Y);   // use default calibration

	// Z Axis //

	Z_TRIS = 1;         // set Z-axis pin as an input
	Z_ANSEL = 1;        // set Z-axis pin as analogue
	accel_default(Z);   // use default calibration
}

S16 accel_scale(S16 reading, S16 min, S16 max, S16 zero)
{
	if (max - min == 0)   // avoid divide by 0
	{
		return ERROR;
	}

	// scale acceleration reading from 'min' <-> '1023' to -100 <-> +100 centered at 'zero'
	return ((200 * (reading - min)) / (max - min)) - zero;
}

void accel_calibrate(enum Direction direction, S16 min, S16 max, S16 zero)
{
	calibration[direction].min = min;
	calibration[direction].max = max;
	calibration[direction].zero = zero;
}

void accel_default(enum Direction direction)
{
	accel_calibrate(direction, ADC_MIN, ADC_MAX, ACCEL_DEFAULT_ZERO);
}

S16 accel_read(enum Direction direction)
{
	return accel_scale(accel_raw(direction), calibration[direction].min, calibration[direction].max, calibration[direction].zero);
}

S16 accel_raw(enum Direction direction)
{
	return adc_read(channels[direction]);
}
