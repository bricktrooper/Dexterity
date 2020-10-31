#ifndef ACCEL_H
#define ACCEL_H

#include <stdbool.h>

#include "dexterity.h"

void accel_init(void);
int accel_mode(enum Mode mode);
int accel_calibrate(struct AccelCalibration * calibration);
int accel_settings(struct AccelCalibration * calibration);
void accel_reset(void);
S16 accel_scaled(enum Direction direction);
S16 accel_raw(enum Direction direction);
S16 accel_read(enum Direction direction);

#endif /* ACCEL_H */
