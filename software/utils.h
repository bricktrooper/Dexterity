#ifndef UTILS_H
#define UTILS_H

#include "dexterity.h"

int sample(Hand * hand);
int raw(void);
int scaled(void);
int upload(Calibration * calibration);
int download(Calibration * calibration);
int deadzone(Hand * hand, int deadzone);

#endif /* UTILS_H */
