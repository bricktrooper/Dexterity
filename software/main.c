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
#define ARGV_MAX          3   // subcommand + argument
#define ARGV_PROGRAM      0
#define ARGV_SUBCOMMAND   1
#define ARGV_ARGUMENT     2

int init(void);
void end(int signal);
void print_usage(void);

// when we calibrate we should 1. have the devie send an ack to affim that it is ready,
// and 2. check the data we just updloaded by requesting a download and comparing it immediately after
// 3. send an ack after the device applies the data

int main(int argc, char ** argv)
{
	char * program = basename(argv[ARGV_PROGRAM]);

	if (argc < ARGV_MIN || argc > ARGV_MAX || argv == NULL)
	{
		log_print(LOG_ERROR, "%s: Invalid arguments\n", program);
		return ERROR;
	}

	char * subcommand = NULL;
	char * argument = NULL;

	if (argc > 1)
	{
		subcommand = argv[ARGV_SUBCOMMAND];
	}

	if (argc > 2)
	{
		argument = argv[ARGV_ARGUMENT];
	}

	enum Command command = command_identify(subcommand);

	if (command == COMMAND_UNKNOWN)
	{
		log_print(LOG_ERROR, "%s: Unknown subcommand '%s'\n", program, subcommand);
		return ERROR;
	}

	int result = init();

	if (result != SUCCESS)
	{
		return result;
	}

	result = command_execute(program, command, argument);
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

	if (serial_open() != SUCCESS)
	{
		log_print(LOG_ERROR, "Initialization failed\n");
		end(ERROR);
	}

	log_print(LOG_SUCCESS, "Initialized Dexterity\n");
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
	log_print(LOG_SUCCESS, "Terminated Dexterity\n");
	exit(code);
}
