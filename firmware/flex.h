#ifndef FLEX_H
#define ACCEL_H

#include <stdbool.h>

#include "dexterity.h"

void flex_init(void);
int flex_mode(enum Mode mode);
int flex_calibrate(struct FlexCalibration * calibration);
int flex_settings(struct FlexCalibration * calibration);
void flex_reset(void);
S16 flex_scaled(enum Finger finger);
S16 flex_raw(enum Finger finger);
S16 flex_read(enum Finger finger);

#endif /* FLEX_H */
