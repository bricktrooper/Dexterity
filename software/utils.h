#ifndef UTILS_H
#define UTILS_H

#include "dexterity.h"

int sample(struct Hand * hand);
int read_file(char * file_name, char ** data, int * size);
int write_file(char * file_name, char * data, int size);

#endif /* UTILS_H */
