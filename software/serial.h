#ifndef SERIAL_H
#define SERIAL_H

int serial_open(void);
int serial_close(void);
int serial_purge(void);
int serial_read(char * data, int size);
int serial_write(char * data, int size);

#endif /* SERIAL_H */
