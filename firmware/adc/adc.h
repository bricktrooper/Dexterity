#ifndef ADC_H
#define ADC_H

#define ADC_MIN   0
#define ADC_MAX   1023

void adc_init(void);
void adc_run(void);
int adc_set_channel(int channel);
int adc_read(int channel);

#endif /* ADC_H */
