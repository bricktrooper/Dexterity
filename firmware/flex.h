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
S16 flex_scale(S16 reading, S16 min, S16 max, S16 zero);
void flex_calibrate(enum Finger finger, S16 min, S16 max, S16 zero);
void flex_default(enum Finger finger);
S16 flex_read(enum Finger finger);
S16 flex_raw(enum Finger finger);

#endif /* FLEX_H */
