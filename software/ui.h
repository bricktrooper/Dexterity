#ifndef UI_H
#define UI_H

#include <stdbool.h>

struct Cursor
{
	int x;
	int y;
};

int ui_init(void);
void ui_cleanup(void);
struct Cursor ui_cursor_locate(void);
bool ui_cursor_valid(struct Cursor cursor);
int ui_test(void);

#endif /* UI_H */
