#ifndef UTILS_H
#define UTILS_H

#include "dexterity.h"

int dexterity(char * subcommand, char ** arguments, int count);
int init(void);
void cleanup(int result);
void end(int code);   // signal handler (calls cleanup() internally)
int sample(Hand * hand);
int raw(void);
int scaled(void);
int upload(Calibration * calibration);
int download(Calibration * calibration);
int deadzone(Hand * hand, int deadzone);

#endif /* UTILS_H */
