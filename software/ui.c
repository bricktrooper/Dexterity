#include <stdio.h>
#include <unistd.h>
#include <ApplicationServices/ApplicationServices.h>

#include "dexterity.h"
#include "log.h"

#include "ui.h"

enum Event
{
	MOVE_CURSOR,
	LEFT_CLICK,
	RIGHT_CLICK,

	NUM_EVENTS
};

static char * EVENT_NAMES [NUM_EVENTS] = {
    "MOVE_CURSOR",
	"LEFT_CLICK",
	"RIGHT_CLICK",
};

static CGEventRef EVENTS [NUM_EVENTS] = {0};   // set all to NULL

static int DEFAULT_X = 0;
static int DEFAULT_Y = 0;

int ui_init(void)
{
	EVENTS[MOVE_CURSOR] = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, CGPointMake(DEFAULT_X, DEFAULT_Y), kCGMouseButtonLeft);
    EVENTS[LEFT_CLICK] = NULL;
    EVENTS[RIGHT_CLICK] = NULL;
    // add more events here

    for (enum Event event = 0; event < NUM_EVENTS; event++)
    {
        if (EVENTS[event] == NULL)
        {
            log_print(LOG_ERROR, "%s(): Failed to create UI event '%s'\n", __func__, EVENT_NAMES[event]);
            ui_cleanup();
			return ERROR;
        }
    }

	return 0;
}

void ui_cleanup(void)
{
    for (enum Event event = 0; event < NUM_EVENTS; event++)
    {
        if (EVENTS[event] != NULL)
        {
            CFRelease(EVENTS[event]);
            EVENTS[event] = NULL;
        }
    }
}

struct Cursor ui_cursor_locate(void)
{
    struct Cursor cursor = { .x = -1, .y = -1 };
    CGEventRef event = CGEventCreate(NULL);

    if (event == NULL)
    {
        log_print(LOG_ERROR, "%s(): Failed to create event to locate cursor\n", __func__);
        return cursor;
    }

    CGPoint point = CGEventGetLocation(event);
    CFRelease(event);

    cursor.x = (int)point.x;
    cursor.y = (int)point.x;

    return cursor;
}

bool ui_cursor_valid(struct Cursor cursor)
{
    if (cursor.x < 0 || cursor.y < 0)
    {
        return false;
    }

    return true;
}

int ui_test(void)
{
    // CGEventRef mouse = EVENTS[MOVE_CURSOR];

	// for (int i = 0; i < 100; i++)
	// {
	// 	CGEventSetLocation(mouse, CGPointMake(DEFAULT_X, DEFAULT_Y + i));
	// 	CGEventPost(kCGHIDEventTap, mouse);   // inject event into HID stream
	// 	usleep(10000);
	// }

    for (int i = 0; i < 100; i++)
    {
        struct Cursor cursor = ui_cursor_locate();
        if (!ui_cursor_valid(cursor))
        {
            printf("Invalid cursor: ");
        }

        printf("x: %d, y: %d\n", cursor.x, cursor.y);
        usleep(10000);
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
