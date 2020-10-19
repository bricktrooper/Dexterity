#include <stdio.h>
#include <unistd.h>
#include <ApplicationServices/ApplicationServices.h>

#include "dexterity.h"
#include "log.h"

#include "mouse.h"

enum Event
{
	MOVE_CURSOR,
	// LEFT_CLICK,
	// RIGHT_CLICK,

	NUM_EVENTS
};

static char * EVENT_NAMES [NUM_EVENTS] = {
    "MOVE_CURSOR",
	// "LEFT_CLICK",
	// "RIGHT_CLICK",
};

static CGEventRef EVENTS [NUM_EVENTS] = {0};   // set all to NULL

static int DEFAULT_X = 0;
static int DEFAULT_Y = 0;

int mouse_init(void)
{
	EVENTS[MOVE_CURSOR] = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, CGPointMake(DEFAULT_X, DEFAULT_Y), kCGMouseButtonLeft);
    // add more events here

    for (enum Event event = 0; event < NUM_EVENTS; event++)
    {
        if (EVENTS[event] == NULL)
        {
            log_print(LOG_ERROR, "%s(): Failed to create UI event '%s'\n", __func__, EVENT_NAMES[event]);
            mouse_cleanup();
			return ERROR;
        }
    }

    log_print(LOG_SUCCESS, "%s(): Created Quartz Events\n", __func__);
	return 0;
}

void mouse_cleanup(void)
{
    for (enum Event event = 0; event < NUM_EVENTS; event++)
    {
        if (EVENTS[event] != NULL)
        {
            CFRelease(EVENTS[event]);
            EVENTS[event] = NULL;
        }
    }

    log_print(LOG_SUCCESS, "%s(): Cleaned up Quartz Events\n", __func__);
}

struct Mouse mouse_get(void)
{
    struct Mouse mouse = { .x = -1, .y = -1 };
    CGEventRef event = CGEventCreate(NULL);

    if (event == NULL)
    {
        log_print(LOG_ERROR, "%s(): Failed to create event to locate mouse cursor\n", __func__);
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

    CGEventRef event = EVENTS[MOVE_CURSOR];

    if (event == NULL)
    {
        log_print(LOG_ERROR, "%s(): Event '%s' is NULL\n", __func__, EVENT_NAMES[MOVE_CURSOR]);
        return ERROR;
    }

    CGEventSetLocation(event, CGPointMake(mouse.x ,mouse.y));
    CGEventPost(kCGHIDEventTap, event);   // inject event into HID stream

    return SUCCESS;
}

bool mouse_valid(struct Mouse mouse)
{
    if (mouse.x < 0 || mouse.y < 0)
    {
        return false;
    }

    return true;
}

int mouse_test(void)
{
    // CGEventRef mouse = EVENTS[MOVE_CURSOR];

	// for (int i = 0; i < 100; i++)
	// {
	// 	CGEventSetLocation(mouse, CGPointMake(DEFAULT_X, DEFAULT_Y + i));
	// 	CGEventPost(kCGHIDEventTap, mouse);   // inject event into HID stream
	// 	usleep(10000);
	// }

    while (1)
    {
        struct Mouse original = mouse_get();

        if (!mouse_valid(original))
        {
            printf("Invalid mouse cursor\n");
            return ERROR;
        }

        struct Mouse modified = original;

        for (int j = 0; j < 100; j++)
        {
            modified.x += 1;
            modified.y += 1;

            printf("x: %d -> %d, y: %d -> %d\n", original.x, modified.x, original.y, modified.y);

            if (mouse_set(modified) != SUCCESS)
            {
                printf("Failed to set mouse\n");
                return ERROR;
            }

            usleep(1000);
        }

        printf("x: %d -> %d, y: %d -> %d\n", original.x, modified.x, original.y, modified.y);
        usleep(1000000);
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
