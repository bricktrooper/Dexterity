/* Various functions to print log messages */

#ifndef LOG_H
#define LOG_H

#include <stdbool.h>

enum LogType
{
	ERROR,
	WARNING,
	SUCCESS,
	DEBUG,
	INFO
};

void log_suppress(enum LogType type, bool suppress);
int log_print(enum LogType type, const char * format, ...);

#endif /* LOG_H */
