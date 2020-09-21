#ifndef LED_H
#define LED_H

void led_init(void);
void led_on(void);
void led_off(void);
int led_blink(int blinks, long duration);

int delay_ms(long duration);

#endif /* LED_H */
