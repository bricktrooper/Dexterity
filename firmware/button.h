#ifndef BUTTON_H
#define BUTTON_H

void button_init(void);
int button_pressed(void);
int button_released(void);
void button_wait_press(void);
void button_wait_release(void);
void button_wait_click(void);

#endif /* BUTTON_H */
