#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "dexterity.h"
#include "colours.h"

#include "log.h"

static char * prefixes [] = {
	RED     "X "  WHITE,
	YELLOW  "! "  WHITE,
	GREEN   "~ "  WHITE,
	BLUE    "# "  WHITE,
	PURPLE  "> "  WHITE
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

	printf("%s", prefixes[type]);
	vprintf(format, args);

    va_end(args);
	return SUCCESS;
}
