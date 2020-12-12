#ifndef FLEX_H
#define ACCEL_H

#include <stdbool.h>

#include "dexterity.h"

void flex_init(void);
int flex_mode(Mode mode);
int flex_calibrate(FlexCalibration * calibration);
int flex_settings(FlexCalibration * calibration);
void flex_reset(void);
S16 flex_scaled(Finger finger);
S16 flex_raw(Finger finger);
S16 flex_read(Finger finger);

#endif /* FLEX_H */
