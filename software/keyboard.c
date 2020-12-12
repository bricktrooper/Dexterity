#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <IOKit/hidsystem/ev_keymap.h>

#include "dexterity.h"
#include "log.h"
#include "special.h"

#include "keyboard.h"

// Additional Virtual Keycodes
#define kVK_MissionControl   160
#define kVK_Dashboard        130

// Mac OS X virtual keycodes
static CGKeyCode const KEYCODES [NUM_KEYS] = {
	kVK_ANSI_A,
	kVK_ANSI_S,
	kVK_ANSI_D,
	kVK_ANSI_F,
	kVK_ANSI_H,
	kVK_ANSI_G,
	kVK_ANSI_Z,
	kVK_ANSI_X,
	kVK_ANSI_C,
	kVK_ANSI_V,
	kVK_ANSI_B,
	kVK_ANSI_Q,
	kVK_ANSI_W,
	kVK_ANSI_E,
	kVK_ANSI_R,
	kVK_ANSI_Y,
	kVK_ANSI_T,
	kVK_ANSI_1,
	kVK_ANSI_2,
	kVK_ANSI_3,
	kVK_ANSI_4,
	kVK_ANSI_6,
	kVK_ANSI_5,
	kVK_ANSI_Equal,
	kVK_ANSI_9,
	kVK_ANSI_7,
	kVK_ANSI_Minus,
	kVK_ANSI_8,
	kVK_ANSI_0,
	kVK_ANSI_RightBracket,
	kVK_ANSI_O,
	kVK_ANSI_U,
	kVK_ANSI_LeftBracket,
	kVK_ANSI_I,
	kVK_ANSI_P,
	kVK_ANSI_L,
	kVK_ANSI_J,
	kVK_ANSI_Quote,
	kVK_ANSI_K,
	kVK_ANSI_Semicolon,
	kVK_ANSI_Backslash,
	kVK_ANSI_Comma,
	kVK_ANSI_Slash,
	kVK_ANSI_N,
	kVK_ANSI_M,
	kVK_ANSI_Period,
	kVK_ANSI_Grave,
	kVK_ANSI_KeypadDecimal,
	kVK_ANSI_KeypadMultiply,
	kVK_ANSI_KeypadPlus,
	kVK_ANSI_KeypadClear,
	kVK_ANSI_KeypadDivide,
	kVK_ANSI_KeypadEnter,
	kVK_ANSI_KeypadMinus,
	kVK_ANSI_KeypadEquals,
	kVK_ANSI_Keypad0,
	kVK_ANSI_Keypad1,
	kVK_ANSI_Keypad2,
	kVK_ANSI_Keypad3,
	kVK_ANSI_Keypad4,
	kVK_ANSI_Keypad5,
	kVK_ANSI_Keypad6,
	kVK_ANSI_Keypad7,
	kVK_ANSI_Keypad8,
	kVK_ANSI_Keypad9,
	kVK_Return,
	kVK_Tab,
	kVK_Space,
	kVK_Delete,
	kVK_Escape,
	kVK_Command,
	kVK_Shift,
	kVK_CapsLock,
	kVK_Option,
	kVK_Control,
	kVK_RightCommand,
	kVK_RightShift,
	kVK_RightOption,
	kVK_RightControl,
	kVK_Function,
	kVK_F17,
	kVK_F18,
	kVK_F19,
	kVK_F20,
	kVK_F5,
	kVK_F6,
	kVK_F7,
	kVK_F3,
	kVK_F8,
	kVK_F9,
	kVK_F11,
	kVK_F13,
	kVK_F16,
	kVK_F14,
	kVK_F10,
	kVK_F12,
	kVK_F15,
	kVK_Help,
	kVK_Home,
	kVK_PageUp,
	kVK_ForwardDelete,
	kVK_F4,
	kVK_End,
	kVK_F2,
	kVK_PageDown,
	kVK_F1,
	kVK_LeftArrow,
	kVK_RightArrow,
	kVK_DownArrow,
	kVK_UpArrow,
	kVK_ISO_Section,
	kVK_MissionControl,
	kVK_Dashboard,

	// SPECIAL KEYCODES //
	// See IOKit/hidsystem/ev_keymap.h for more keys
	// Requires NSEvent Objective-C code
	NX_KEYTYPE_SOUND_UP,
	NX_KEYTYPE_SOUND_DOWN,
	NX_KEYTYPE_BRIGHTNESS_UP,
	NX_KEYTYPE_BRIGHTNESS_DOWN,
	NX_KEYTYPE_MUTE,
	NX_KEYTYPE_LAUNCH_PANEL,
	NX_KEYTYPE_PLAY,
	NX_KEYTYPE_NEXT,
	NX_KEYTYPE_PREVIOUS,
	NX_KEYTYPE_FAST,
	NX_KEYTYPE_REWIND,
	NX_KEYTYPE_ILLUMINATION_UP,
	NX_KEYTYPE_ILLUMINATION_DOWN,
	NX_KEYTYPE_ILLUMINATION_TOGGLE
};

char * key_string(Key key)
{
	switch (key)
	{
		case KEY_A:                           return "A";
		case KEY_S:                           return "S";
		case KEY_D:                           return "D";
		case KEY_F:                           return "F";
		case KEY_H:                           return "H";
		case KEY_G:                           return "G";
		case KEY_Z:                           return "Z";
		case KEY_X:                           return "X";
		case KEY_C:                           return "C";
		case KEY_V:                           return "V";
		case KEY_B:                           return "B";
		case KEY_Q:                           return "Q";
		case KEY_W:                           return "W";
		case KEY_E:                           return "E";
		case KEY_R:                           return "R";
		case KEY_Y:                           return "Y";
		case KEY_T:                           return "T";
		case KEY_1:                           return "1";
		case KEY_2:                           return "2";
		case KEY_3:                           return "3";
		case KEY_4:                           return "4";
		case KEY_6:                           return "6";
		case KEY_5:                           return "5";
		case KEY_EQUAL:                       return "EQUAL";
		case KEY_9:                           return "9";
		case KEY_7:                           return "7";
		case KEY_MINUS:                       return "MINUS";
		case KEY_8:                           return "8";
		case KEY_0:                           return "0";
		case KEY_RIGHT_BRACKET:               return "RIGHT_BRACKET";
		case KEY_O:                           return "O";
		case KEY_U:                           return "U";
		case KEY_LEFT_BRACKET:                return "LEFT_BRACKET";
		case KEY_I:                           return "I";
		case KEY_P:                           return "P";
		case KEY_L:                           return "L";
		case KEY_J:                           return "J";
		case KEY_QUOTE:                       return "QUOTE";
		case KEY_K:                           return "K";
		case KEY_SEMICOLON:                   return "SEMICOLON";
		case KEY_BACKSLASH:                   return "BACKSLASH";
		case KEY_COMMA:                       return "COMMA";
		case KEY_SLASH:                       return "SLASH";
		case KEY_N:                           return "N";
		case KEY_M:                           return "M";
		case KEY_PERIOD:                      return "PERIOD";
		case KEY_GRAVE:                       return "GRAVE";
		case KEY_KEYPAD_DECIMAL:              return "KEYPAD_DECIMAL";
		case KEY_KEYPAD_MULTIPLY:             return "KEYPAD_MULTIPLY";
		case KEY_KEYPAD_PLUS:                 return "KEYPAD_PLUS";
		case KEY_KEYPAD_CLEAR:                return "KEYPAD_CLEAR";
		case KEY_KEYPAD_DIVIDE:               return "KEYPAD_DIVIDE";
		case KEY_KEYPAD_ENTER:                return "KEYPAD_ENTER";
		case KEY_KEYPAD_MINUS:                return "KEYPAD_MINUS";
		case KEY_KEYPAD_EQUALS:               return "KEYPAD_EQUALS";
		case KEY_KEYPAD_0:                    return "KEYPAD_0";
		case KEY_KEYPAD_1:                    return "KEYPAD_1";
		case KEY_KEYPAD_2:                    return "KEYPAD_2";
		case KEY_KEYPAD_3:                    return "KEYPAD_3";
		case KEY_KEYPAD_4:                    return "KEYPAD_4";
		case KEY_KEYPAD_5:                    return "KEYPAD_5";
		case KEY_KEYPAD_6:                    return "KEYPAD_6";
		case KEY_KEYPAD_7:                    return "KEYPAD_7";
		case KEY_KEYPAD_8:                    return "KEYPAD_8";
		case KEY_KEYPAD_9:                    return "KEYPAD_9";
		case KEY_RETURN:                      return "RETURN";
		case KEY_TAB:                         return "TAB";
		case KEY_SPACE:                       return "SPACE";
		case KEY_BACKSPACE:                   return "BACKSPACE";
		case KEY_ESCAPE:                      return "ESCAPE";
		case KEY_COMMAND:                     return "COMMAND";
		case KEY_SHIFT:                       return "SHIFT";
		case KEY_CAPS_LOCK:                   return "CAPS_LOCK";
		case KEY_ALT:                         return "ALT";
		case KEY_CONTROL:                     return "CONTROL";
		case KEY_RIGHT_COMMAND:               return "RIGHT_COMMAND";
		case KEY_RIGHT_SHIFT:                 return "RIGHT_SHIFT";
		case KEY_RIGHT_ALT:                   return "RIGHT_ALT";
		case KEY_RIGHT_CONTROL:               return "RIGHT_CONTROL";
		case KEY_FUNCTION:                    return "FUNCTION";
		case KEY_F17:                         return "F17";
		case KEY_F18:                         return "F18";
		case KEY_F19:                         return "F19";
		case KEY_F20:                         return "F20";
		case KEY_F5:                          return "F5";
		case KEY_F6:                          return "F6";
		case KEY_F7:                          return "F7";
		case KEY_F3:                          return "F3";
		case KEY_F8:                          return "F8";
		case KEY_F9:                          return "F9";
		case KEY_F11:                         return "F11";
		case KEY_F13:                         return "F13";
		case KEY_F16:                         return "F16";
		case KEY_F14:                         return "F14";
		case KEY_F10:                         return "F10";
		case KEY_F12:                         return "F12";
		case KEY_F15:                         return "F15";
		case KEY_HELP:                        return "HELP";
		case KEY_HOME:                        return "HOME";
		case KEY_PAGE_UP:                     return "PAGE_UP";
		case KEY_DELETE:                      return "DELETE";
		case KEY_F4:                          return "F4";
		case KEY_END:                         return "END";
		case KEY_F2:                          return "F2";
		case KEY_PAGE_DOWN:                   return "PAGE_DOWN";
		case KEY_F1:                          return "F1";
		case KEY_LEFT_ARROW:                  return "LEFT_ARROW";
		case KEY_RIGHT_ARROW:                 return "RIGHT_ARROW";
		case KEY_DOWN_ARROW:                  return "DOWN_ARROW";
		case KEY_UP_ARROW:                    return "UP_ARROW";
		case KEY_ISO_SECTION:                 return "ISO_SECTION";
		case KEY_MISSION_CONTROL:             return "MISSION_CONTROL";
		case KEY_DASHBOARD:                   return "DASHBOARD";
		case KEY_VOLUME_UP:                   return "VOLUME_UP";
		case KEY_VOLUME_DOWN:                 return "VOLUME_DOWN";
		case KEY_BRIGHTNESS_UP:               return "BRIGHTNESS_UP";
		case KEY_BRIGHTNESS_DOWN:             return "BRIGHTNESS_DOWN";
		case KEY_MUTE:                        return "MUTE";
		case KEY_LAUNCH_PANEL:                return "LAUNCH_PANEL";
		case KEY_PAUSE_PLAY:                  return "PAUSE_PLAY";
		case KEY_NEXT:                        return "NEXT";
		case KEY_PREVIOUS:                    return "PREVIOUS";
		case KEY_FAST_FORWARD:                return "FAST_FORWARD";
		case KEY_REWIND:                      return "REWIND";
		case KEY_KEYBOARD_BACKLIGHT_UP:       return "KEYBOARD_BACKLIGHT_UP";
		case KEY_KEYBOARD_BACKLIGHT_DOWN:     return "KEYBOARD_BACKLIGHT_DOWN";
		case KEY_KEYBOARD_BACKLIGHT_TOGGLE:   return "KEYBOARD_BACKLIGHT_TOGGLE";
		default:                              return "INVALID";
	}
}

int keyboard_press(Key key)
{
	if (key < 0 || key >= NUM_KEYS)
	{
		log(LOG_ERROR, "Invalid keycode '%d'\n", key);
		return ERROR;
	}

	// if special keycode then call Objective-C functions
	if (key >= KEY_VOLUME_UP)
	{
		return keyboard_special_press(KEYCODES[key]);
	}

	CGEventRef event = CGEventCreateKeyboardEvent(
							NULL,             // source of the event
							KEYCODES[key],    // OS X virtual keycode
							true              // key state (true == pressed, false == released)
							);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create keyboard press event\n");
		return ERROR;
	}

	CGEventPost(kCGHIDEventTap, event);   // inject event into HID stream
	CFRelease(event);

	return SUCCESS;
}

int keyboard_release(Key key)
{
	if (key < 0 || key >= NUM_KEYS)
	{
		log(LOG_ERROR, "Invalid keycode '%d'\n", key);
		return ERROR;
	}

	// if special keycode then call Objective-C functions
	if (key >= KEY_VOLUME_UP)
	{
		return keyboard_special_release(KEYCODES[key]);
	}

	CGEventRef event = CGEventCreateKeyboardEvent(
							NULL,             // source of the event
							KEYCODES[key],    // OS X virtual keycode
							false             // key state (true == pressed, false == released)
							);

	if (event == NULL)
	{
		log(LOG_ERROR, "Failed to create keyboard release event\n");
		return ERROR;
	}

	CGEventPost(kCGHIDEventTap, event);   // inject event into HID stream
	CFRelease(event);

	return SUCCESS;
}

int keyboard_tap(Key key)
{
	if (key < 0 || key >= NUM_KEYS)
	{
		log(LOG_ERROR, "Invalid keycode '%d'\n", key);
		return ERROR;
	}

	if (keyboard_press(key) == ERROR)
	{
		log(LOG_ERROR, "Failed to press key\n");
		return ERROR;
	}

	usleep(50);

	if (keyboard_release(key) == ERROR)
	{
		log(LOG_ERROR, "Failed to release key\n");
		return ERROR;
	}

	usleep(50);
	return SUCCESS;
}

int keyboard_type(Key * keys, int length)
{
	if (keys == NULL || length < 0)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	for (int i = 0; i < length; i++)
	{
		Key key = keys[i];

		if (key < 0 || key >= NUM_KEYS)
		{
			log(LOG_ERROR, "Invalid keycode '%d' (key #%d)\n", key, i);
			return ERROR;
		}

		if (keyboard_tap(key) == ERROR)
		{
			log(LOG_ERROR, "Failed to tap key '%s'\n", key_string(key));
			return ERROR;
		}
	}

	return SUCCESS;
}

int keyboard_combo(Key * keys, int length)
{
	if (keys == NULL || length < 0)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	for (int i = 0; i < length; i++)
	{
		Key key = keys[i];

		if (key < 0 || key >= NUM_KEYS)
		{
			log(LOG_ERROR, "Invalid keycode '%d' (key #%d)\n", key, i);
			return ERROR;
		}

		if (keyboard_press(key) == ERROR)
		{
			log(LOG_ERROR, "Failed to press key '%s'\n", key_string(key));
			return ERROR;
		}

		usleep(50);
	}

	for (int i = 0; i < length; i++)
	{
		if (keyboard_release(keys[i]) == ERROR)
		{
			log(LOG_ERROR, "Failed to release key '%s'\n", key_string(keys[i]));
			return ERROR;
		}

		usleep(50);
	}

	return SUCCESS;
}
