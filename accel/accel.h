#ifndef ACCEL_H
#define ACCEL_H

void accel_init(void);
int accel_scale(int reading);
int accel_x(void);
int accel_y(void);
int accel_z(void);

#endif /* ACCEL_H */
