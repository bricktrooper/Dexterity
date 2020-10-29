#ifndef UTILS_H
#define UTILS_H

#include "dexterity.h"

int sample(struct Hand * hand);
int raw(void);
int scaled(void);
int upload(struct Calibration * calibration);
int download(struct Calibration * calibration);
int read_file(char * file_name, char ** data, int * size);
int write_file(char * file_name, char * data, int size);
int average(int * data, int elements, float * avg);

#endif /* UTILS_H */
