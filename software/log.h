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

typedef enum LogType LogType;

#define log(...)   log_print(__FILE__, __LINE__, __func__, __VA_ARGS__)

void log_suppress(LogType type, bool suppress);
void log_trace(bool enabled);
int log_print(char const * file, int line, char const * function, LogType type, char const * format, ...) __attribute__ ((format (printf, 5, 6)));

#endif /* LOG_H */
