#ifndef ADC_H
#define ADC_H

void adc_init(void);
void adc_run(void);
int adc_set_channel(int channel);
int adc_read(int channel);

#endif /* ADC_H */
