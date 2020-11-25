#ifndef UTILS_H
#define UTILS_H

#include "dexterity.h"

int sample(struct Hand * hand);
int raw(void);
int scaled(void);
int upload(struct Calibration * calibration);
int download(struct Calibration * calibration);
int deadzone(struct Hand * hand, int deadzone);

#endif /* UTILS_H */
