#ifndef ACCEL_H
#define ACCEL_H

#include <stdbool.h>

#include "dexterity.h"

void accel_init(void);
int accel_mode(Mode mode);
int accel_calibrate(AccelCalibration * calibration);
int accel_settings(AccelCalibration * calibration);
void accel_reset(void);
S16 accel_scaled(Direction direction);
S16 accel_raw(Direction direction);
S16 accel_read(Direction direction);

#endif /* ACCEL_H */
