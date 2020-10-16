#ifndef ACCEL_H
#define ACCEL_H

#include <stdbool.h>

#include "dexterity.h"

void accel_init(void);
void accel_enable_scaling(bool enable);
void accel_calibrate(enum Direction direction, S16 min, S16 max, S16 zero);
void accel_default(enum Direction direction);
S16 accel_scaled(enum Direction direction);
S16 accel_raw(enum Direction direction);
S16 accel_read(enum Direction direction);

#endif /* ACCEL_H */
