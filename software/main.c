#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"
#include "command.h"

#define ARGV_MIN          1   // no subcommand
#define ARGV_MAX          4   // subcommand + arguments
#define ARGV_PROGRAM      0
#define ARGV_SUBCOMMAND   1
#define ARGV_ARGUMENTS    2

int main(int argc, char ** argv)
{
	// LOGGING SETTINGS //

	log_suppress(LOG_ERROR, false);
	log_suppress(LOG_WARNING, false);
	log_suppress(LOG_SUCCESS, false);
	log_suppress(LOG_INFO, false);
	log_suppress(LOG_DEBUG, true);

	log_trace(false);   // don't print the source of log messages

	// PARSE ARGUMENTS //

	if (argc < ARGV_MIN)
	{
		log(LOG_ERROR, "Insufficient arguments\n");
		print_usage(COMMAND_DEXTERITY, false);
		return ERROR;
	}

	if (argc > ARGV_MAX)
	{
		log(LOG_ERROR, "Too many arguments\n");
		print_usage(COMMAND_DEXTERITY, false);
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

	// RUN DEXTERITY //

	return dexterity(subcommand, arguments, count);
}
