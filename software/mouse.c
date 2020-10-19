#include <stdio.h>
#include <unistd.h>
#include <ApplicationServices/ApplicationServices.h>

#include "dexterity.h"
#include "log.h"

#include "mouse.h"

#define DEFAULT_X   0   // default X position of mouse cursor
#define DEFAULT_Y   0   // default Y position of mouse cursor

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
    CGEventRef event = CGEventCreate(NULL);

    if (event == NULL)
    {
        log_print(LOG_ERROR, "%s(): Failed to create event\n", __func__);
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

    // create an event
    CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, CGPointMake(DEFAULT_X, DEFAULT_Y), kCGMouseButtonLeft);

    if (event == NULL)
    {
        log_print(LOG_ERROR, "%s(): Failed to create event\n", __func__);
        return ERROR;
    }

    CGEventSetLocation(event, CGPointMake(mouse.x ,mouse.y));
    CGEventPost(kCGHIDEventTap, event);   // inject event into HID stream
    CFRelease(event);                     // destroy the event

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

int mouse_test(void)
{
    while (1)
    {
        if (mouse_move(1, 2) != SUCCESS)
        {
            printf("ERROR\n");
            return ERROR;
        }

        usleep(10000);

        // printf("x: %d -> %d, y: %d -> %d\n", original.x, modified.x, original.y, modified.y);
    }

    return SUCCESS;

    // // Move to 200x200
    // CGEventRef move1 = CGEventCreateMouseEvent(
    //     NULL, kCGEventMouseMoved,
    //     CGPointMake(200, 200),
    //     kCGMouseButtonLeft // ignored
    // );
    // // Move to 250x250
    // CGEventRef move2 = CGEventCreateMouseEvent(
    //     NULL, kCGEventMouseMoved,
    //     CGPointMake(250, 250),
    //     kCGMouseButtonLeft // ignored
    // );
    // // Left button down at 250x250
    // CGEventRef click1_down = CGEventCreateMouseEvent(
    //     NULL, kCGEventLeftMouseDown,
    //     CGPointMake(250, 250),
    //     kCGMouseButtonLeft
    // );
    // // Left button up at 250x250
    // CGEventRef click1_up = CGEventCreateMouseEvent(
    //     NULL, kCGEventLeftMouseUp,
    //     CGPointMake(250, 250),
    //     kCGMouseButtonLeft
    // );

    // // Now, execute these events with an interval to make them noticeable
    // CGEventPost(kCGHIDEventTap, move1);
    // sleep(1);
    // CGEventPost(kCGHIDEventTap, move2);
    // sleep(1);
    // CGEventPost(kCGHIDEventTap, click1_down);
    // CGEventPost(kCGHIDEventTap, click1_up);

    // // Release the events
    // CFRelease(click1_up);
    // CFRelease(click1_down);
    // CFRelease(move2);
    // CFRelease(move1);

    // return 0;
}
