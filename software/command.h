#ifndef COMMAND_H
#define COMMAND_H

enum Command
{
	COMMAND_RUN,         // run the usual program
	COMMAND_SAMPLE,      // continuously sample the sensors and dump the readings in the console
	COMMAND_CALIBRATE,   // start an interactive calibration (requires an output file)
	COMMAND_UPLOAD,      // upload a calibration file to the device (requires a calibration file)
	COMMAND_DOWNLOAD,    // download a calibration from the device (optionally save to an output file, otherwise print the calibration)
	COMMAND_MODE,         // set the scaling mode of ADC readings
	COMMAND_RECORD,         // set the scaling mode of ADC readings

	// Future commands
	// COMMAND_RECORD    // record a new gesture
	// some more commands....

	NUM_COMMANDS,

	COMMAND_UNKNOWN      // the command entered was unknown
};

enum Command command_identify(char * subcommand);
int command_execute(char * program, enum Command command, char * argument);

#endif /* COMMAND_H */
