#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "dexterity.h"

int calibration_import(char * file_name, struct Calibration * calibration);
int calibration_export(char * file_name, struct Calibration * calibration);
int calibration_download(struct Calibration * calibration);
int calibration_upload(struct Calibration * calibration);
int calibration_interactive(struct Calibration * calibration);
int calibration_print(struct Calibration * calibration);

#endif /* CALIBRATION_H */
