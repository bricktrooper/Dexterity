#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <AppKit/NSEvent.h>

#include "dexterity.h"
#include "log.h"

#include "special.h"

#define TYPE                    NSEventTypeSystemDefined
#define LOCATION                NSZeroPoint
#define KEY_PRESS_FLAG          0xa00
#define KEY_RELEASE_FLAG        0xb00
#define DATA1_KEY_PRESS         0xa
#define DATA1_KEY_RELEASE       0xb
#define TIMESTAMP               0
#define WINDOW                  0
#define CONTEXT                 nil
#define SUBTYPE                 8
#define DATA2                  -1

#define DATA1_KEYCODE_SHIFT     16
#define DATA1_KEY_STATE_SHIFT   8

int keyboard_special_press(SpecialKeyCode keycode)
{
	@autoreleasepool
	{
		NSEvent * event = [NSEvent otherEventWithType: TYPE
		                           location: LOCATION
		                           modifierFlags: KEY_PRESS_FLAG
		                           timestamp: TIMESTAMP
		                           windowNumber: WINDOW
		                           context: CONTEXT
		                           subtype: SUBTYPE
		                           data1: (keycode << DATA1_KEYCODE_SHIFT)| (DATA1_KEY_PRESS << DATA1_KEY_STATE_SHIFT)
		                           data2: DATA2
		                           ];

		if (event == nil)
		{
			log(LOG_ERROR, "Failed to create special keyboard press event\n");
			return ERROR;
		}

		CGEventPost(kCGHIDEventTap, [event CGEvent]);
	}

	return SUCCESS;
}

int keyboard_special_release(SpecialKeyCode keycode)
{
	@autoreleasepool
	{
		NSEvent * event = [NSEvent otherEventWithType: TYPE
		                           location: LOCATION
		                           modifierFlags: KEY_RELEASE_FLAG
		                           timestamp: TIMESTAMP
		                           windowNumber: WINDOW
		                           context: CONTEXT
		                           subtype: SUBTYPE
		                           data1: (keycode << DATA1_KEYCODE_SHIFT)| (DATA1_KEY_RELEASE << DATA1_KEY_STATE_SHIFT)
		                           data2: DATA2
		                           ];

		if (event == nil)
		{
			log(LOG_ERROR, "Failed to create special keyboard release event\n");
			return ERROR;
		}

		CGEventPost(kCGHIDEventTap, [event CGEvent]);
	}

	return SUCCESS;
}
