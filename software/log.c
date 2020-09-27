#include <stdio.h>
#include <stdarg.h>

#include "colours.h"
#include "log.h"

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

int log_error(const char * format, ...)
{
	if (format == NULL)
	{
		return ERROR;
	}

	va_list args;
    va_start(args, format);

	printf(RED "ERROR: " WHITE);
	vprintf(format, args);

    va_end(args);
	return SUCCESS;
}

int log_warning(const char * format, ...)
{
	if (format == NULL)
	{
		return ERROR;
	}

	va_list args;
    va_start(args, format);

	printf(YELLOW "WARNING: " WHITE);
	vprintf(format, args);

    va_end(args);
	return SUCCESS;
}

int log_success(const char * format, ...)
{
	if (format == NULL)
	{
		return ERROR;
	}

	va_list args;
    va_start(args, format);

	printf(GREEN "SUCCESS: " WHITE);
	vprintf(format, args);

    va_end(args);
	return SUCCESS;
}

int log_debug(const char * format, ...)
{
	if (format == NULL)
	{
		return ERROR;
	}

	va_list args;
    va_start(args, format);

	printf(BLUE "DEBUG: " WHITE);
	vprintf(format, args);

    va_end(args);
	return SUCCESS;
}

int log_info(const char * format, ...)
{
	if (format == NULL)
	{
		return ERROR;
	}

	va_list args;
    va_start(args, format);

	printf(PURPLE "-> " WHITE);
	vprintf(format, args);

    va_end(args);
	return SUCCESS;
}

