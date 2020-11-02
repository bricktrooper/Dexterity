#include <ApplicationServices/ApplicationServices.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"

#include "mouse.h"

#define X_MIN   0
#define Y_MIN   0
#define X_MAX   1920
#define Y_MAX   1080

static CGEventRef mouse_create_event(CGEventType type)
{
	struct Mouse mouse = mouse_get();   // get current cursor location

	if (!mouse_valid(mouse))
	{
		log_print(LOG_WARNING, "%s(): Invalid mouse cursor location (%d,%d)\n", __func__, mouse.x, mouse.y);
		mouse_correct(&mouse);
	}

	// create an event
	CGEventRef event = CGEventCreateMouseEvent(
		                    NULL,                            // source of the event
		                    type,                            // type of mouse event
		                    CGPointMake(mouse.x, mouse.y),   // location of mouse event (use current location)
		                    kCGMouseButtonLeft               // ignored
		                    );

	if (event == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to create mouse event\n", __func__);
		return NULL;
	}

	return event;
}

static void mouse_destroy_event(CGEventRef event)
{
	CFRelease(event);   // destroy the event
}

bool mouse_valid(struct Mouse mouse)
{
	if (mouse.x < X_MIN || mouse.x > X_MAX || mouse.y < Y_MIN || mouse.y > Y_MAX)
	{
		return false;
	}

	return true;
}

int mouse_correct(struct Mouse * mouse)
{
	if (mouse == NULL)
	{
		log_print(LOG_ERROR, "%s(): Mouse is NULL\n", __func__);
		return ERROR;
	}

	if (mouse->x < X_MIN)
	{
		mouse->x = X_MIN;
	}
	else if (mouse->x > X_MAX)
	{
		mouse->x = X_MAX;
	}

	if (mouse->y < Y_MIN)
	{
		mouse->y = Y_MIN;
	}
	else if (mouse->y > Y_MAX)
	{
		mouse->y = Y_MAX;
	}

	log_print(LOG_INFO, "%s(): Corrected cursor position to edge (%d,%d)\n", __func__, mouse->x, mouse->y);
	return SUCCESS;
}

struct Mouse mouse_get(void)
{
	struct Mouse mouse = { .x = -1, .y = -1 };

	// Do not use mouse_create_event() here because
	// it calls this function to get the cursor location.
	// Calling mouse_create_event() here will cause infinite recursion

	CGEventRef event = CGEventCreate(NULL);

	if (event == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to create event to get cursor position\n", __func__);
		return mouse;
	}

	CGPoint point = CGEventGetLocation(event);
	CFRelease(event);

	mouse.x = (int)point.x;
	mouse.y = (int)point.y;

	return mouse;
}

int mouse_set(struct Mouse mouse)
{
	int rc = SUCCESS;

	if (!mouse_valid(mouse))
	{
		log_print(LOG_WARNING, "%s(): Invalid mouse cursor location (%d,%d)\n", __func__, mouse.x, mouse.y);
		mouse_correct(&mouse);
		rc = WARNING;
	}

	CGEventRef event = mouse_create_event(kCGEventMouseMoved);

	if (event == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to create event to move cursor\n", __func__);
		return ERROR;
	}

	CGEventSetLocation(event, CGPointMake(mouse.x, mouse.y));   // update the location of the cursor
	CGEventPost(kCGHIDEventTap, event);                         // inject event into HID stream
	mouse_destroy_event(event);

	return rc;
}

int mouse_move(int x_offset, int y_offset)
{
	int rc = SUCCESS;
	struct Mouse mouse = mouse_get();

	if (!mouse_valid(mouse))
	{
		log_print(LOG_WARNING, "%s(): Invalid mouse cursor location (%d,%d)\n", __func__, mouse.x, mouse.y);
		mouse_correct(&mouse);
		rc = WARNING;
	}

	mouse.x += x_offset;
	mouse.y += y_offset;

	if (mouse_set(mouse) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to move mouse cursor to (%d,%d)\n", __func__, mouse.x, mouse.y);
		return ERROR;
	}

	return rc;
}

int mouse_press(enum MouseButton button)
{
	if (button != MOUSE_BUTTON_LEFT && button != MOUSE_BUTTON_RIGHT)
	{
		log_print(LOG_ERROR, "%s(): Invalid mouse button\n", __func__);
		return ERROR;
	}

	CGEventRef event = mouse_create_event((button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseDown : kCGEventRightMouseDown);

	if (event == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to create event to press mouse button\n", __func__);
		return ERROR;
	}

	CGEventPost(kCGHIDEventTap, event);
	mouse_destroy_event(event);

	return SUCCESS;
}

int mouse_release(enum MouseButton button)
{
	if (button != MOUSE_BUTTON_LEFT && button != MOUSE_BUTTON_RIGHT)
	{
		log_print(LOG_ERROR, "%s(): Invalid mouse button\n", __func__);
		return ERROR;
	}

	CGEventRef event = mouse_create_event((button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseUp : kCGEventRightMouseUp);

	if (event == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to create event to release mouse button\n", __func__);
		return ERROR;
	}

	CGEventPost(kCGHIDEventTap, event);
	mouse_destroy_event(event);

	return SUCCESS;
}

int mouse_single_click(enum MouseButton button)
{
	if (button != MOUSE_BUTTON_LEFT && button != MOUSE_BUTTON_RIGHT)
	{
		log_print(LOG_ERROR, "%s(): Invalid mouse button\n", __func__);
		return ERROR;
	}

	CGEventType press = (button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseDown : kCGEventRightMouseDown;
	CGEventType release = (button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseUp : kCGEventRightMouseUp;

	CGEventRef event = mouse_create_event(press);

	if (event == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to create event to single click mouse button\n", __func__);
		return ERROR;
	}

	CGEventSetIntegerValueField(event, kCGMouseEventClickState, 1);   // single click
	CGEventPost(kCGHIDEventTap, event);
	CGEventSetType(event, release);
	CGEventPost(kCGHIDEventTap, event);

	mouse_destroy_event(event);
	return SUCCESS;
}

int mouse_double_click(enum MouseButton button)
{
	if (button != MOUSE_BUTTON_LEFT && button != MOUSE_BUTTON_RIGHT)
	{
		log_print(LOG_ERROR, "%s(): Invalid mouse button\n", __func__);
		return ERROR;
	}

	CGEventType press = (button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseDown : kCGEventRightMouseDown;
	CGEventType release = (button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseUp : kCGEventRightMouseUp;

	CGEventRef event = mouse_create_event(press);

	if (event == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to create event to double click mouse button\n", __func__);
		return ERROR;
	}

	CGEventSetIntegerValueField(event, kCGMouseEventClickState, 2);   // double click (for triple click use 3)

	// first click
	CGEventPost(kCGHIDEventTap, event);
	CGEventSetType(event, release);
	CGEventPost(kCGHIDEventTap, event);

	// second click
	CGEventSetType(event, press);
	CGEventPost(kCGHIDEventTap, event);
	CGEventSetType(event, release);
	CGEventPost(kCGHIDEventTap, event);

	mouse_destroy_event(event);
	return SUCCESS;
}

int mouse_drag(enum MouseButton button, int x_offset, int y_offset)
{
	int rc = SUCCESS;
	struct Mouse mouse = mouse_get();

	if (!mouse_valid(mouse))
	{
		log_print(LOG_WARNING, "%s(): Invalid mouse cursor location (%d,%d)\n", __func__, mouse.x, mouse.y);
		mouse_correct(&mouse);
		rc = WARNING;
	}

	mouse.x += x_offset;
	mouse.y += y_offset;

	if (!mouse_valid(mouse))
	{
		log_print(LOG_WARNING, "%s(): Invalid mouse cursor location (%d,%d)\n", __func__, mouse.x, mouse.y);
		mouse_correct(&mouse);
		rc = WARNING;
	}

	CGEventRef event = mouse_create_event((button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseDragged : kCGEventRightMouseDragged);

	if (event == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to create event to drag cursor\n", __func__);
		return ERROR;
	}

	CGEventSetLocation(event, CGPointMake(mouse.x ,mouse.y));   // update the location of the cursor
	CGEventPost(kCGHIDEventTap, event);                         // inject event into HID stream
	mouse_destroy_event(event);

	return rc;
}

int mouse_scroll(enum ScrollDirection direction, S32 speed)
{
	if (direction != MOUSE_SCROLL_UP && direction != MOUSE_SCROLL_DOWN)
	{
		log_print(LOG_ERROR, "%s(): Invalid scroll direction\n", __func__);
		return ERROR;
	}

	if (speed < 0)
	{
		log_print(LOG_ERROR, "%s(): Scroll speed '%d' cannot be negative\n", __func__, speed);
		return ERROR;
	}

	CGEventRef event = CGEventCreateScrollWheelEvent(
		                    NULL,                      // source of the event
		                    kCGScrollEventUnitLine,    // scroll lines instead of pixels (smooth scrolling)
		                    1,                         // only one scroll wheel
		                    (speed * direction)        // scroll magnitude (+ == UP, - == DOWN)
		                    );

	if (event == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to create mouse scroll event\n", __func__);
		return ERROR;
	}

	CGEventPost(kCGHIDEventTap, event);
	mouse_destroy_event(event);

	return SUCCESS;
}
