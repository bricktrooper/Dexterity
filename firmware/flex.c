#include <xc.h>
#include <pic16f690.h>

#include "utils.h"
#include "pins.h"
#include "adc.h"

#include "flex.h"

#define FLEX_MAX   800
#define FLEX_MIN   500

void flex_init(void)
{
	ADCON0bits.VCFG = 0;   // use VDD as VREF for conversions (5V)

	// FINGER 1 //

	F1_TRIS = 1;      // set finger 1 pin as an input
	F1_ANSEL = 1;     // set finger 1 pin as analogue

	// FINGER 2 //

	F2_TRIS = 1;      // set finger 2 pin as an input
	F2_ANSEL = 1;     // set finger 2 pin as analogue

	// FINGER 1 //

	F3_TRIS = 1;      // set finger 3 pin as an input
	F3_ANSEL = 1;     // set finger 3 pin as analogue

	// FINGER 4 //

	F4_TRIS = 1;      // set finger 4 pin as an input
	F4_ANSEL = 1;     // set finger 4 pin as analogue

	// FINGER 5 //

	F5_TRIS = 1;      // set finger 5 pin as an input
	F5_ANSEL = 1;     // set finger 5 pin as analogue
}

int flex_scale(int reading)
{
	// scale flex reading from 0 <-> 1023 to 0 <-> 100
	return (int)(100 * ((double)(reading - FLEX_MIN) / (double)(FLEX_MAX - FLEX_MIN)));
}

int flex_f1(void)
{
	return flex_scale(adc_read(F1_CHANNEL));
}

int flex_f2(void)
{
	return flex_scale(adc_read(F2_CHANNEL));
}

int flex_f3(void)
{
	return flex_scale(adc_read(F3_CHANNEL));
}

int flex_f4(void)
{
	return flex_scale(adc_read(F4_CHANNEL));
}

int flex_f5(void)
{
	return flex_scale(adc_read(F5_CHANNEL));
}
