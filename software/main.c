#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "dexterity.h"
#include "log.h"
#include "calibration.h"
#include "command.h"
#include "serial.h"

#define ARGV_MIN          1   // no subcommand
#define ARGV_MAX          4   // subcommand + arguments
#define ARGV_PROGRAM      0
#define ARGV_SUBCOMMAND   1
#define ARGV_ARGUMENTS    2

int init(void);
void end(int code);
void print_usage(void);

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

	enum Command command = command_identify(subcommand);

	if (command == COMMAND_UNKNOWN)
	{
		log(LOG_ERROR, "Unknown subcommand '%s'\n", subcommand);
		return ERROR;
	}

	int result = init();

	if (result == ERROR)
	{
		return result;
	}

	result = command_execute(command, arguments, count);
	end(result);

	return SUCCESS;
}

int init(void)
{
	signal(SIGINT, end);   // register signal handler for CTRL+C

	log_suppress(LOG_ERROR, false);
	log_suppress(LOG_WARNING, false);
	log_suppress(LOG_SUCCESS, false);
	log_suppress(LOG_DEBUG, false);
	log_suppress(LOG_INFO, true);

	log_trace(false);

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
	if (code == SIGINT)
	{
		printf("\n");
	}

	serial_purge();
	serial_close();
	log(LOG_SUCCESS, "Terminated Dexterity\n");
	exit(code);
}
