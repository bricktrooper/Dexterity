#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "dexterity.h"
#include "colours.h"

#include "log.h"

static char * prefixes [] = {
	RED     "X"  WHITE,
	YELLOW  "!"  WHITE,
	GREEN   "~"  WHITE,
	BLUE    "#"  WHITE,
	PURPLE  ">"  WHITE
};

static bool suppressed [] = {
	false,
	false,
	false,
	false,
	false
};

static bool trace = false;

void log_suppress(enum LogType type, bool suppress)
{
	suppressed[type] = suppress;
}

void log_trace(bool enabled)
{
	trace = enabled;
}

int log_print(char const * file, int line, char const * function, enum LogType type, char const * format, ...)
{
	if (suppressed[type])
	{
		return SUCCESS;
	}

	if (format == NULL || function == NULL || file == NULL)
	{
		return ERROR;
	}

	printf("%s", prefixes[type]);

	if (trace)
	{
		printf(" [%s:%d:%s]", file, line, function);
	}

	printf(" ");

	va_list args;
    va_start(args, format);
	vprintf(format, args);
    va_end(args);

	return SUCCESS;
}
