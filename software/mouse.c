#include <stdio.h>
#include <unistd.h>
#include <ApplicationServices/ApplicationServices.h>

#include "dexterity.h"
#include "log.h"

#include "mouse.h"

static CGEventRef mouse_create_event(CGEventType type)
{
    struct Mouse mouse = mouse_get();   // get current cursor location

    if (!mouse_valid(mouse))
    {
        log_print(LOG_ERROR, "%s(): Invalid mouse cursor location '(%d, %d)'\n", __func__, mouse.x, mouse.y);
        return NULL;
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
        log_print(LOG_ERROR, "%s(): Failed to create event\n", __func__);
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
    if (mouse.x < 0 || mouse.y < 0)
    {
        return false;
    }

    return true;
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
    if (!mouse_valid(mouse))
    {
        log_print(LOG_ERROR, "%s(): Invalid mouse cursor location '(%d, %d)'\n", __func__, mouse.x, mouse.y);
        return ERROR;
    }

    CGEventRef event = mouse_create_event(kCGEventMouseMoved);

    if (event == NULL)
    {
        log_print(LOG_ERROR, "%s(): Failed to create event to move cursor\n", __func__);
        return ERROR;
    }

    CGEventSetLocation(event, CGPointMake(mouse.x ,mouse.y));   // update the location of the cursor
    CGEventPost(kCGHIDEventTap, event);                         // inject event into HID stream
    mouse_destroy_event(event);

    return SUCCESS;
}

int mouse_move(int x_offset, int y_offset)
{
    struct Mouse mouse = mouse_get();

    if (!mouse_valid(mouse))
    {
        log_print(LOG_ERROR, "%s(): Invalid mouse cursor location '(%d, %d)'\n", __func__, mouse.x, mouse.y);
        return ERROR;
    }

    mouse.x += x_offset;
    mouse.y += y_offset;

    if (mouse_set(mouse) != SUCCESS)
    {
        log_print(LOG_ERROR, "%s(): Failed to move mouse cursor to '(%d, %d)'\n", __func__, mouse.x, mouse.y);
        return ERROR;
    }

    return SUCCESS;
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
        log_print(LOG_ERROR, "%s(): Failed to create event to single click mouse button\n", __func__);
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

int mouse_drag(enum MouseButton button, int x_offset, int y_offset);

int mouse_test(void)
{
    if (mouse_single_click(MOUSE_BUTTON_LEFT) != SUCCESS)
    {
        printf("ERROR\n");
        return ERROR;
    }


    // for (int i = 0; i < 50; i++)
    // {
    //     if (mouse_move(0, 2) != SUCCESS)
    //     {
    //         printf("ERROR\n");
    //         return ERROR;
    //     }

    //     usleep(10000);

    //     // printf("x: %d -> %d, y: %d -> %d\n", original.x, modified.x, original.y, modified.y);
    // }

    return SUCCESS;
}
