#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

#include "dexterity.h"
#include "log.h"

#include "keyboard.h"

// Additional Virtual Keycodes
#define kVK_BrightnessDown   107
#define kVK_BrightnessUp     113
#define kVK_MissionControl   160
#define kVK_Dashboard        130

// Mac OS X virtual keycodes
static const CGKeyCode KEYCODES [NUM_KEYS] = {
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
	kVK_VolumeUp,
	kVK_VolumeDown,
	kVK_Mute,
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
	kVK_BrightnessDown,
	kVK_BrightnessUp,
	kVK_MissionControl,
	kVK_Dashboard
};

static char * KEY_NAMES [NUM_KEYS] = {
	"A",
	"S",
	"D",
	"F",
	"H",
	"G",
	"Z",
	"X",
	"C",
	"V",
	"B",
	"Q",
	"W",
	"E",
	"R",
	"Y",
	"T",
	"1",
	"2",
	"3",
	"4",
	"6",
	"5",
	"EQUAL",
	"9",
	"7",
	"MINUS",
	"8",
	"0",
	"RIGHT_BRACKET",
	"O",
	"U",
	"LEFT_BRACKET",
	"I",
	"P",
	"L",
	"J",
	"QUOTE",
	"K",
	"SEMICOLON",
	"BACKSLASH",
	"COMMA",
	"SLASH",
	"N",
	"M",
	"PERIOD",
	"GRAVE",
	"KEYPAD_DECIMAL",
	"KEYPAD_MULTIPLY",
	"KEYPAD_PLUS",
	"KEYPAD_CLEAR",
	"KEYPAD_DIVIDE",
	"KEYPAD_ENTER",
	"KEYPAD_MINUS",
	"KEYPAD_EQUALS",
	"KEYPAD_0",
	"KEYPAD_1",
	"KEYPAD_2",
	"KEYPAD_3",
	"KEYPAD_4",
	"KEYPAD_5",
	"KEYPAD_6",
	"KEYPAD_7",
	"KEYPAD_8",
	"KEYPAD_9",
	"RETURN",
	"TAB",
	"SPACE",
	"BACKSPACE",
	"ESCAPE",
	"COMMAND",
	"SHIFT",
	"CAPS_LOCK",
	"ALT",
	"CONTROL",
	"RIGHT_COMMAND",
	"RIGHT_SHIFT",
	"RIGHT_ALT",
	"RIGHT_CONTROL",
	"FUNCTION",
	"F17",
	"VOLUME_UP",
	"VOLUME_DOWN",
	"MUTE",
	"F18",
	"F19",
	"F20",
	"F5",
	"F6",
	"F7",
	"F3",
	"F8",
	"F9",
	"F11",
	"F13",
	"F16",
	"F14",
	"F10",
	"F12",
	"F15",
	"HELP",
	"HOME",
	"PAGE_UP",
	"DELETE",
	"F4",
	"END",
	"F2",
	"PAGE_DOWN",
	"F1",
	"LEFT_ARROW",
	"RIGHT_ARROW",
	"DOWN_ARROW",
	"UP_ARROW",
	"ISO_SECTION",
	"BRIGHTNESS_DOWN",
	"BRIGHTNESS_UP",
	"MISSION_CONTROL",
	"DASHBOARD"
};

int keyboard_press(enum Key key)
{
    if (key < 0 || key >= NUM_KEYS)
    {
        log(LOG_ERROR, "Invalid keycode '%d'\n", key);
        return ERROR;
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

int keyboard_release(enum Key key)
{
    if (key < 0 || key >= NUM_KEYS)
    {
        log(LOG_ERROR, "Invalid keycode '%d'\n", key);
        return ERROR;
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

int keyboard_tap(enum Key key)
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

    log(LOG_INFO, "Tapped '%s'\n", KEY_NAMES[key]);
    return SUCCESS;
}

int keyboard_type(enum Key * keys, int length)
{
    if (keys == NULL || length < 0)
    {
        log(LOG_ERROR, "Invalid arguments\n");
        return ERROR;
    }

    for (int i = 0; i < length; i++)
    {
        enum Key key = keys[i];

        if (key < 0 || key >= NUM_KEYS)
        {
            log(LOG_ERROR, "Invalid keycode '%d' (key #%d)\n", key, i);
            return ERROR;
        }

        if (keyboard_tap(key) == ERROR)
        {
            log(LOG_ERROR, "Failed to tap key '%s'\n", KEY_NAMES[key]);
            return ERROR;
        }
    }

    return SUCCESS;
}

int keyboard_combo(enum Key * keys, int length)
{
    if (keys == NULL || length < 0)
    {
        log(LOG_ERROR, "Invalid arguments\n");
        return ERROR;
    }

    for (int i = 0; i < length; i++)
    {
        enum Key key = keys[i];

        if (key < 0 || key >= NUM_KEYS)
        {
            log(LOG_ERROR, "Invalid keycode '%d' (key #%d)\n", key, i);
            return ERROR;
        }

        if (keyboard_press(key) == ERROR)
        {
            log(LOG_ERROR, "Failed to press key '%s'\n", KEY_NAMES[key]);
            return ERROR;
        }

        usleep(50);
    }

    for (int i = 0; i < length; i++)
    {
        if (keyboard_release(keys[i]) == ERROR)
        {
            log(LOG_ERROR, "Failed to release key '%s'\n", KEY_NAMES[keys[i]]);
            return ERROR;
        }

        usleep(50);
    }

    return SUCCESS;
}
