#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>

struct Mouse
{
	int x;
	int y;
};

int mouse_init(void);
void mouse_cleanup(void);
struct Mouse mouse_get(void);
int mouse_set(struct Mouse mouse);
bool mouse_valid(struct Mouse mouse);
int mouse_test(void);

#endif /* MOUSE_H */
