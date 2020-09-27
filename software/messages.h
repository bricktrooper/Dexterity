/* Various functions to print log messages */

#ifndef MESSAGES_H
#define MESSAGES_H

int print_error(const char * format, ...);
int print_warning(const char * format, ...);
int print_success(const char * format, ...);
int print_debug(const char * format, ...);
int print_info(const char * format, ...);

#endif /* MESSAGES_H */
