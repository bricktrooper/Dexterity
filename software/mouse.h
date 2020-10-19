#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>

struct Mouse
{
	int x;
	int y;
};

bool mouse_valid(struct Mouse mouse);
struct Mouse mouse_get(void);
int mouse_set(struct Mouse mouse);
int mouse_move(int x_offset, int y_offset);
int mouse_test(void);

#endif /* MOUSE_H */
