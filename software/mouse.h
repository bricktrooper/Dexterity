#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>

enum MouseButton
{
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT
};

enum ScrollDirection
{
	MOUSE_SCROLL_UP = 1,    // (+) == UP
	MOUSE_SCROLL_DOWN = -1  // (-) == DOWN
};

struct Mouse
{
	int x;
	int y;
};

bool mouse_valid(struct Mouse mouse);
int mouse_correct(struct Mouse * mouse);
struct Mouse get_display_dimensions(void);
struct Mouse mouse_get(void);
int mouse_set(struct Mouse mouse);
int mouse_move(int x_offset, int y_offset);
int mouse_press(enum MouseButton button);
int mouse_release(enum MouseButton button);
int mouse_single_click(enum MouseButton button);
int mouse_double_click(enum MouseButton button);
int mouse_drag(enum MouseButton button, int x_offset, int y_offset);
int mouse_scroll(enum ScrollDirection direction, S32 speed);

#endif /* MOUSE_H */
