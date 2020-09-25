#include <xc.h>
#include <pic16f690.h>

#include "utils.h"
#include "pins.h"
#include "adc.h"

#include "accel.h"

#define ACCEL_DEFAULT_ZERO   100

static struct Analogue sensors [3];

void accel_init(void)
{
	adc_set_vref(EXTERNAL);   // use external VREF for conversions (3.3V)

	// X Axis //

	X_TRIS = 1;      // set X-axis pin as an input
	X_ANSEL = 1;     // set X-axis pin as analogue

	sensors[X].channel = X_CHANNEL;
	accel_default(X);

	// Y Axis //

	Y_TRIS = 1;      // set Y-axis pin as an input
	Y_ANSEL = 1;     // set Y-axis pin as analogue

	sensors[Y].channel = Y_CHANNEL;
	accel_default(Y);

	// Z Axis //

	Z_TRIS = 1;      // set Z-axis pin as an input
	Z_ANSEL = 1;     // set Z-axis pin as analogue

	sensors[Z].channel = Z_CHANNEL;
	accel_default(Z);
}

int accel_scale(int reading, int min, int max, int zero)
{
	if (max - min == 0)   // avoid divide by 0
	{
		return ERROR;
	}

	// scale acceleration reading from 'min' <-> '1023' to -100 <-> +100 centered at 'zero'
	return ((200 * (reading - min)) / (max - min)) - zero;
}

void accel_calibrate(enum Direction direction, int min, int max, int zero)
{
	sensors[direction].min = min;
	sensors[direction].max = max;
	sensors[direction].zero = zero;
}

void accel_default(enum Direction direction)
{
	accel_calibrate(direction, ADC_MIN, ADC_MAX, ACCEL_DEFAULT_ZERO);
}

int accel_read(enum Direction direction)
{
	return accel_scale(accel_raw(direction), sensors[direction].min, sensors[direction].max, sensors[direction].zero);
}

int accel_raw(enum Direction direction)
{
	return adc_read(sensors[direction].channel);
}
