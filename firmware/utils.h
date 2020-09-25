#ifndef UTILS_H
#define UTILS_H

#define null       0

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

struct Analogue
{
	int channel;
	int max;
	int min;
	int zero;
};

int delay_s(long duration);
int delay_ms(long duration);
int delay_us(long duration);

#endif /* UTILS_H */
