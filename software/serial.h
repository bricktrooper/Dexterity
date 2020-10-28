#ifndef SERIAL_H
#define SERIAL_H

int serial_open(void);
int serial_close(void);
bool serial_is_open(void);
int serial_purge(void);
int serial_read(void * buffer, int size);
int serial_write(void * buffer, int size);
int serial_write_message(enum Message message);
enum Message serial_read_message(void);

#endif /* SERIAL_H */
