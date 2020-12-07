#ifndef COMMAND_H
#define COMMAND_H

enum Command
{
	COMMAND_RUN,         // run the usual program
	COMMAND_SAMPLE,      // continuously sample the sensors and dump the readings in the console
	COMMAND_CALIBRATE,   // start an interactive calibration (requires an output file)
	COMMAND_UPLOAD,      // upload a calibration file to the device (requires a calibration file)
	COMMAND_DOWNLOAD,    // download a calibration from the device (optionally save to an output file, otherwise print the calibration)
	COMMAND_RAW,         // disable scaling of ADC readings
	COMMAND_SCALED,      // enable scaling of ADC readings
	COMMAND_RECORD,      // record an n-phase gesture
	COMMAND_HELP,        // print command usage information

	NUM_COMMANDS,

	COMMAND_UNKNOWN,     // the command entered was unknown
	COMMAND_DEXTERITY    // the main command
};

void print_usage(enum Command command, bool align);
char * command_string(enum Command command);
void command_free_gestures(void);
enum Command command_identify(char * subcommand);
int command_execute(enum Command command, char ** arguments, int count);

#endif /* COMMAND_H */
