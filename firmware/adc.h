#ifndef ADC_H
#define ADC_H

#define ADC_MIN   0
#define ADC_MAX   1023

enum Vref
{
	INTERNAL = 0,
	EXTERNAL = 1
};

void adc_init(void);
void adc_run(void);
int adc_read(int channel);
int adc_set_channel(int channel);
void adc_set_vref(enum Vref vref);

#endif /* ADC_H */
