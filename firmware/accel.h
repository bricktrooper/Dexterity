#ifndef ACCEL_H
#define ACCEL_H

#include "dexterity.h"

void accel_init(void);
S16 accel_scale(S16 reading, S16 min, S16 max, S16 zero);
void accel_calibrate(enum Direction direction, S16 min, S16 max, S16 zero);
void accel_default(enum Direction direction);
S16 accel_read(enum Direction direction);
S16 accel_raw(enum Direction direction);

#endif /* ACCEL_H */
