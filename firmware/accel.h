#ifndef ACCEL_H
#define ACCEL_H

enum Direction
{
	X,
	Y,
	Z
};

void accel_init(void);
int accel_scale(int reading, int min, int max, int zero);
void accel_calibrate(enum Direction direction, int min, int max, int zero);
void accel_default(enum Direction direction);
int accel_read(enum Direction direction);
int accel_raw(enum Direction direction);

#endif /* ACCEL_H */
