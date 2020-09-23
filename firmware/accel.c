#include <xc.h>
#include <pic16f690.h>

#include "utils.h"
#include "pins.h"
#include "adc.h"

#include "accel.h"

void accel_init(void)
{
	ADCON0bits.VCFG = 1;   // use external VREF for conversions (3.3V)

	// X Axis //

	X_TRIS = 1;      // set X-axis pin as an input
	X_ANSEL = 1;     // set X-axis pin as analogue

	// Y Axis //

	Y_TRIS = 1;      // set Y-axis pin as an input
	Y_ANSEL = 1;     // set Y-axis pin as analogue

	// Z Axis //

	Z_TRIS = 1;      // set Z-axis pin as an input
	Z_ANSEL = 1;     // set Z-axis pin as analogue
}

int accel_scale(int reading)
{
	/* scale acceleration reading from
	 * 0 <-> 1023 (511.5 = 0 m/s^2) to
	 * -100 <-> +100 (0 = 0 m/s^2) */
	return (int)(200 * ((double)(reading) / (double)(ADC_MAX - ADC_MIN))) - 100;
}

int accel_x(void)
{
	return accel_scale(adc_read(X_CHANNEL));
}

int accel_y(void)
{
	return accel_scale(adc_read(Y_CHANNEL));
}

int accel_z(void)
{
	return accel_scale(adc_read(Z_CHANNEL));
}
