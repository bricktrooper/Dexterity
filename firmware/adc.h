#ifndef ADC_H
#define ADC_H

#define ADC_MIN   0
#define ADC_MAX   1023

enum VREF
{
	VREF_INTERNAL = 0,
	VREF_EXTERNAL = 1
};

void adc_init(void);
void adc_run(void);
S16 adc_read(int channel);
int adc_set_channel(int channel);
void adc_set_vref(enum VREF vref);

#endif /* ADC_H */
