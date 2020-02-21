#include <xc.h>
#include <pic16f690.h>

#include "adc.h"

#define null       0

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

void adc_init(void)
{

	ADCON0bits.ADFM = 1;      // right justify result
	ADCON0bits.VCFG = 0;      // use VDD as VREF for conversions
	ADCON0bits.CHS = 11;      // set default channel to AN11
	ADCON0bits.GO_DONE = 0;   // set ADC to idle
	ADCON0bits.ADON = 1;      // enable ADC hardware

	ADCON1bits.ADCS = 0b111;  // use internal oscillator for ADC conversion clock
}

void adc_run(void)
{
	GO_DONE = 1;              // start the ADC conversion
	while (GO_DONE == 1);     // wait for the conversion to finish
}

int adc_set_channel(int channel)
{
	if (channel < 0 || channel > 11)
	{
		return ERROR;
	}

	ADCON0bits.CHS = channel;   // update ADC input channel
	return SUCCESS;
}

int adc_read(int channel)
{
	if (adc_set_channel(channel))
	{
		return ERROR;
	}

	adc_run();
	return (ADRESH << 8) + ADRESL;   // calculate result of conversion
}