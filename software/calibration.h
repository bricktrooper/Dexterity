#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "dexterity.h"

int calibration_import(char * filename, struct Calibration * calibration);
int calibration_export(char * filename, struct Calibration * calibration);
int calibration_download(struct Calibration * calibration);
int calibration_upload(struct Calibration * calibration);
int calibration_interactive();

#endif /* CALIBRATION_H */
