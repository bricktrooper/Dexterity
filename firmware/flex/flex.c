#include <xc.h>
#include <pic16f690.h>

#include "adc.h"
#include "flex.h"

#define null       0

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

#define F1_CHANNEL   2
#define F2_CHANNEL   4
#define F3_CHANNEL   5
#define F4_CHANNEL   6
#define F5_CHANNEL   10

#define F1_TRIS    TRISA2
#define F2_TRIS    TRISC0
#define F3_TRIS    TRISC1
#define F4_TRIS    TRISC2
#define F5_TRIS    TRISB4

#define F1_ANSEL   ANS2
#define F2_ANSEL   ANS4
#define F3_ANSEL   ANS5
#define F4_ANSEL   ANS6
#define F5_ANSEL   ANS10

#define FLEX_MAX   1015
#define FLEX_MIN   997

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
