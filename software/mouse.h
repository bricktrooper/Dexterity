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

typedef enum MouseButton MouseButton;
typedef enum ScrollDirection ScrollDirection;

typedef struct Mouse Mouse;

struct Mouse
{
	int x;
	int y;
};

bool mouse_valid(Mouse mouse);
int mouse_correct(Mouse * mouse);
Mouse get_display_dimensions(void);
Mouse mouse_get(void);
int mouse_set(Mouse mouse);
int mouse_move(int dx, int dy);
int mouse_press(MouseButton button);
int mouse_release(MouseButton button);
int mouse_single_click(MouseButton button);
int mouse_double_click(MouseButton button);
int mouse_drag(MouseButton button, int dx, int dy);
int mouse_scroll(ScrollDirection direction, S32 speed);

#endif /* MOUSE_H */
