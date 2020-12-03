#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

#include "dexterity.h"
#include "log.h"
#include "serial.h"
#include "command.h"
#include "gesture.h"

#define ARGV_MIN          1   // no subcommand
#define ARGV_MAX          4   // subcommand + arguments
#define ARGV_PROGRAM      0
#define ARGV_SUBCOMMAND   1
#define ARGV_ARGUMENTS    2

struct Gesture * GESTURES = NULL;   // point to the list of gestures (freed on exit)
int NUM_GESTURES = 0;               // the number of gestures in the list

int dexterity(char * subcommand, char ** arguments, int count);
int init(void);
void end(int code);

// when we calibrate we should 1. have the devie send an ack to affim that it is ready,
// and 2. check the data we just updloaded by requesting a download and comparing it immediately after
// 3. send an ack after the device applies the data

int main(int argc, char ** argv)
{
	if (argc < ARGV_MIN)
	{
		log(LOG_ERROR, "Insufficient arguments\n");
		return ERROR;
	}

	if (argc > ARGV_MAX)
	{
		log(LOG_ERROR, "Too many arguments\n");
		return ERROR;
	}

	if (argv == NULL)
	{
		log(LOG_ERROR, "Invalid argument vector\n");
		return ERROR;
	}

	char * subcommand = NULL;
	char ** arguments = NULL;
	int count = 0;

	if (argc > 1)
	{
		subcommand = argv[ARGV_SUBCOMMAND];
	}

	if (argc > 2)
	{
		count = argc - 2;
		arguments = &argv[ARGV_ARGUMENTS];
	}

	end(dexterity(subcommand, arguments, count));
	return SUCCESS;   // dead code for silencing compiler warnings
}

int dexterity(char * subcommand, char ** arguments, int count)
{
	enum Command command = command_identify(subcommand);

	if (command == COMMAND_UNKNOWN)
	{
		log(LOG_ERROR, "Unknown subcommand '%s'\n", subcommand);
		return ERROR;
	}

	if (init() == ERROR)
	{
		return ERROR;
	}

	return command_execute(command, arguments, count);
}

int init(void)
{
	// SIGNAL HANDLERS //
	signal(SIGINT, end);    // Ctrl + C
    signal(SIGHUP, end);    // hangup Hang up detected on controlling terminal or death of controlling process
    signal(SIGQUIT, end);   // Ctrl + D
    signal(SIGFPE, end);    // arithmetic error
    signal(SIGKILL, end);   // kill
    signal(SIGTERM, end);   // terminate

	// LOGGING SETTINGS //
	log_suppress(LOG_ERROR, false);
	log_suppress(LOG_WARNING, false);
	log_suppress(LOG_SUCCESS, false);
	log_suppress(LOG_DEBUG, false);
	log_suppress(LOG_INFO, true);

	log_trace(true);

	// SERIAL PORT //
	if (serial_open() == ERROR)
	{
		log(LOG_ERROR, "Initialization failed\n");
		end(ERROR);
	}

	log(LOG_SUCCESS, "Initialized Dexterity\n");
	return SUCCESS;
}

void end(int code)
{
	printf("\n");

	// SERIAL PORT //
	if (serial_is_open())
	{
		serial_purge();
		serial_close();
	}

	// GESTURES //
	gesture_destroy(GESTURES, NUM_GESTURES);
	GESTURES = NULL;
	NUM_GESTURES = 0;

	log(LOG_SUCCESS, "Terminated Dexterity (%d)\n", code);
	exit(code);   // end program
}
