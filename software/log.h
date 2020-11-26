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

#define log(...)   log_print(__func__, __FILE__, __LINE__, __VA_ARGS__)

void log_suppress(enum LogType type, bool suppress);
void log_trace(bool enabled);
int log_print(const char * function, const char * file, int line, enum LogType type, const char * format, ...) __attribute__ ((format (printf, 5, 6)));

#endif /* LOG_H */
