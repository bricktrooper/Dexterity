#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CGDisplayConfiguration.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"

#include "mouse.h"

static CGEventRef mouse_create_event(CGEventType type)
{
	Mouse mouse = mouse_get();   // get current cursor location

	if (!mouse_valid(mouse))
	{
		log(LOG_WARNING, "Invalid mouse cursor location (%d,%d)\n", mouse.x, mouse.y);
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
		log(LOG_ERROR, "Failed to create mouse event\n");
		return NULL;
	}

	return event;
}

static void mouse_destroy_event(CGEventRef event)
{
	CFRelease(event);   // destroy the event
}

Mouse get_display_dimensions(void)
{
	Mouse dimensions;
	CGDirectDisplayID display = CGMainDisplayID();
	dimensions.x = CGDisplayPixelsWide(display);
	dimensions.y =  CGDisplayPixelsHigh(display);
	return dimensions;
}

bool mouse_valid(Mouse mouse)
{
	Mouse dimensions = get_display_dimensions();

	if (mouse.x < 0 || mouse.x > dimensions.x ||
	    mouse.y < 0 || mouse.y > dimensions.y)
	{
		return false;
	}

	return true;
}

int mouse_correct(Mouse * mouse)
{
	if (mouse == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	Mouse dimensions = get_display_dimensions();

	if (mouse->x < 0)
	{
		mouse->x = 0;
	}
	else if (mouse->x > dimensions.x)
	{
		mouse->x = dimensions.x;
	}

	if (mouse->y < 0)
	{
		mouse->y = 0;
	}
	else if (mouse->y > dimensions.y)
	{
		mouse->y = dimensions.y;
	}

	log(LOG_DEBUG, "Corrected cursor position to edge (%d,%d)\n", mouse->x, mouse->y);
	return SUCCESS;
}

Mouse mouse_get(void)
{
	Mouse mouse = { .x = -1, .y = -1 };

	// Do not use mouse_create_event() here because
	// it calls this function to get the cursor location.
	// Calling mouse_create_event() here will cause infinite recursion

	CGEventRef event = CGEventCreate(NULL);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create event to get cursor position\n");
		return mouse;
	}

	CGPoint point = CGEventGetLocation(event);
	CFRelease(event);

	mouse.x = (int)point.x;
	mouse.y = (int)point.y;

	return mouse;
}

int mouse_set(Mouse mouse)
{
	int result = SUCCESS;

	if (!mouse_valid(mouse))
	{
		log(LOG_DEBUG, "Invalid mouse cursor location (%d,%d)\n", mouse.x, mouse.y);
		mouse_correct(&mouse);
		result = WARNING;
	}

	CGEventRef event = mouse_create_event(kCGEventMouseMoved);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create event to move cursor\n");
		return ERROR;
	}

	CGEventSetLocation(event, CGPointMake(mouse.x, mouse.y));   // update the location of the cursor
	CGEventPost(kCGHIDEventTap, event);                         // inject event into HID stream
	mouse_destroy_event(event);

	return result;
}

int mouse_move(int dx, int dy)
{
	int result = SUCCESS;
	Mouse mouse = mouse_get();

	if (!mouse_valid(mouse))
	{
		log(LOG_WARNING, "Invalid mouse cursor location (%d,%d)\n", mouse.x, mouse.y);
		mouse_correct(&mouse);
		result = WARNING;
	}

	mouse.x += dx;
	mouse.y += dy;

	CGEventRef event = mouse_create_event(kCGEventMouseMoved);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create event to move cursor\n");
		return ERROR;
	}

	CGEventSetLocation(event, CGPointMake(mouse.x, mouse.y));     // update the location of the cursor
	CGEventSetDoubleValueField(event, kCGMouseEventDeltaX, dx);   // set delta X
	CGEventSetDoubleValueField(event, kCGMouseEventDeltaY, dy);   // set delta Y
	CGEventPost(kCGHIDEventTap, event);                           // inject event into HID stream
	mouse_destroy_event(event);

	return result;
}

int mouse_press(MouseButton button)
{
	if (button != MOUSE_BUTTON_LEFT && button != MOUSE_BUTTON_RIGHT)
	{
		log(LOG_ERROR, "Invalid mouse button\n");
		return ERROR;
	}

	CGEventRef event = mouse_create_event((button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseDown : kCGEventRightMouseDown);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create event to press mouse button\n");
		return ERROR;
	}

	CGEventPost(kCGHIDEventTap, event);
	mouse_destroy_event(event);

	return SUCCESS;
}

int mouse_release(MouseButton button)
{
	if (button != MOUSE_BUTTON_LEFT && button != MOUSE_BUTTON_RIGHT)
	{
		log(LOG_ERROR, "Invalid mouse button\n");
		return ERROR;
	}

	CGEventRef event = mouse_create_event((button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseUp : kCGEventRightMouseUp);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create event to release mouse button\n");
		return ERROR;
	}

	CGEventPost(kCGHIDEventTap, event);
	mouse_destroy_event(event);

	return SUCCESS;
}

int mouse_single_click(MouseButton button)
{
	if (button != MOUSE_BUTTON_LEFT && button != MOUSE_BUTTON_RIGHT)
	{
		log(LOG_ERROR, "Invalid mouse button\n");
		return ERROR;
	}

	CGEventType press = (button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseDown : kCGEventRightMouseDown;
	CGEventType release = (button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseUp : kCGEventRightMouseUp;

	CGEventRef event = mouse_create_event(press);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create event to single click mouse button\n");
		return ERROR;
	}

	CGEventSetIntegerValueField(event, kCGMouseEventClickState, 1);   // single click
	CGEventPost(kCGHIDEventTap, event);
	CGEventSetType(event, release);
	CGEventPost(kCGHIDEventTap, event);

	mouse_destroy_event(event);
	return SUCCESS;
}

int mouse_double_click(MouseButton button)
{
	if (button != MOUSE_BUTTON_LEFT && button != MOUSE_BUTTON_RIGHT)
	{
		log(LOG_ERROR, "Invalid mouse button\n");
		return ERROR;
	}

	CGEventType press = (button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseDown : kCGEventRightMouseDown;
	CGEventType release = (button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseUp : kCGEventRightMouseUp;

	CGEventRef event = mouse_create_event(press);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create event to double click mouse button\n");
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

int mouse_drag(MouseButton button, int dx, int dy)
{
	int result = SUCCESS;
	Mouse mouse = mouse_get();

	if (!mouse_valid(mouse))
	{
		log(LOG_WARNING, "Invalid mouse cursor location (%d,%d)\n", mouse.x, mouse.y);
		mouse_correct(&mouse);
		result = WARNING;
	}

	mouse.x += dx;
	mouse.y += dy;

	if (!mouse_valid(mouse))
	{
		log(LOG_WARNING, "Invalid mouse cursor location (%d,%d)\n", mouse.x, mouse.y);
		mouse_correct(&mouse);
		result = WARNING;
	}

	CGEventRef event = mouse_create_event((button == MOUSE_BUTTON_LEFT) ? kCGEventLeftMouseDragged : kCGEventRightMouseDragged);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create event to drag cursor\n");
		return ERROR;
	}

	CGEventSetLocation(event, CGPointMake(mouse.x, mouse.y));     // update the location of the cursor
	CGEventSetDoubleValueField(event, kCGMouseEventDeltaX, dx);   // set delta X
	CGEventSetDoubleValueField(event, kCGMouseEventDeltaY, dy);   // set delta Y
	CGEventPost(kCGHIDEventTap, event);                           // inject event into HID stream
	mouse_destroy_event(event);

	return result;
}

int mouse_scroll(ScrollDirection direction, S32 speed)
{
	if (direction != MOUSE_SCROLL_UP && direction != MOUSE_SCROLL_DOWN)
	{
		log(LOG_ERROR, "Invalid scroll direction\n");
		return ERROR;
	}

	if (speed < 0)
	{
		log(LOG_ERROR, "Scroll speed '%d' cannot be negative\n", speed);
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
		log(LOG_ERROR, "Failed to create mouse scroll event\n");
		return ERROR;
	}

	CGEventPost(kCGHIDEventTap, event);
	mouse_destroy_event(event);

	return SUCCESS;
}
