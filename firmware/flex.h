#ifndef FLEX_H
#define ACCEL_H

#include "dexterity.h"

void flex_init(void);
S16 flex_scale(S16 reading, S16 min, S16 max, S16 zero);
void flex_enable_scaling(bool enable);
void flex_calibrate(enum Finger finger, S16 min, S16 max, S16 zero);
void flex_default(enum Finger finger);
S16 flex_scaled(enum Finger finger);
S16 flex_raw(enum Finger finger);
S16 flex_read(enum Finger finger);

#endif /* FLEX_H */
