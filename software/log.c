#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "colours.h"
#include "log.h"

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

static char * prefixes [] = {
	RED     "ERROR: "    WHITE,
	YELLOW  "WARNING: "  WHITE,
	GREEN   "SUCCESS: "  WHITE,
	BLUE    "DEBUG: "    WHITE,
	PURPLE  "-> "        WHITE
};

static int supressed [] = {
	false,
	false,
	false,
	false,
	false
};

void log_suppress(enum LogType type, bool suppress)
{
	supressed[type] = suppress;
}

int log_print(enum LogType type, const char * format, ...)
{
	if (format == NULL)
	{
		return ERROR;
	}

	if (supressed[type])
	{
		return SUCCESS;
	}

	va_list args;
    va_start(args, format);

	printf(prefixes[type]);
	vprintf(format, args);

    va_end(args);
	return SUCCESS;
}
