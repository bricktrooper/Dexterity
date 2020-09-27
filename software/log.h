/* Various functions to print log messages */

#ifndef LOG_H
#define LOG_H

int log_error(const char * format, ...);
int log_warning(const char * format, ...);
int log_success(const char * format, ...);
int log_debug(const char * format, ...);
int log_info(const char * format, ...);

#endif /* LOG_H */
