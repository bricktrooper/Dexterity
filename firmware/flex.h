#ifndef FLEX_H
#define ACCEL_H

#include <stdbool.h>

#include "dexterity.h"

void flex_init(void);
void flex_enable_scaling(bool enable);
S16 flex_get_range(void);
void flex_set_range(S16 range);
void flex_calibrate(enum Finger finger, S16 min, S16 max, S16 zero);
void flex_default(enum Finger finger);
int flex_settings(enum Finger finger, struct Parameters * param);
S16 flex_scaled(enum Finger finger);
S16 flex_raw(enum Finger finger);
S16 flex_read(enum Finger finger);

#endif /* FLEX_H */
