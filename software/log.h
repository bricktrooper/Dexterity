/* Various functions to print log messages */

#ifndef LOG_H
#define LOG_H

#include <stdbool.h>

enum LogType
{
	LOG_ERROR,
	LOG_WARNING,
	LOG_SUCCESS,
	LOG_DEBUG,
	LOG_INFO
};

void log_suppress(enum LogType type, bool suppress);
int log_print(enum LogType type, const char * format, ...) __attribute__ ((format (printf, 2, 3)));

#endif /* LOG_H */
