#ifndef FLEX_H
#define ACCEL_H

enum Finger
{
	F1,
	F2,
	F3,
	F4,
	F5
};

void flex_init(void);
int flex_scale(int reading, int min, int max, int zero);
void flex_calibrate(enum Finger finger, int min, int max, int zero);
void flex_default(enum Finger finger);
int flex_read(enum Finger finger);
int flex_raw(enum Finger finger);

#endif /* FLEX_H */
