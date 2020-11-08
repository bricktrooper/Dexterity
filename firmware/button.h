#ifndef BUTTON_H
#define BUTTON_H

void button_init(void);
int button_pressed(void);
int button_released(void);
void button_wait_for_press(void);
void button_wait_for_release(void);
void button_wait_for_click(void);

#endif /* BUTTON_H */
