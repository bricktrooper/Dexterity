#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "dexterity.h"

int calibration_import(char * file_name, Calibration * calibration);
int calibration_export(char * file_name, Calibration * calibration);
int calibration_download(Calibration * calibration);
int calibration_upload(Calibration * calibration);
int calibration_interactive(Calibration * calibration);
int calibration_print(Calibration * calibration);

#endif /* CALIBRATION_H */
