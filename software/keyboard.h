#ifndef KEYBOARD_H
#define KEYBOARD_H

enum Key
{
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_F,
	KEY_H,
	KEY_G,
	KEY_Z,
	KEY_X,
	KEY_C,
	KEY_V,
	KEY_B,
	KEY_Q,
	KEY_W,
	KEY_E,
	KEY_R,
	KEY_Y,
	KEY_T,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_6,
	KEY_5,
	KEY_EQUAL,
	KEY_9,
	KEY_7,
	KEY_MINUS,
	KEY_8,
	KEY_0,
	KEY_RIGHT_BRACKET,
	KEY_O,
	KEY_U,
	KEY_LEFT_BRACKET,
	KEY_I,
	KEY_P,
	KEY_L,
	KEY_J,
	KEY_QUOTE,
	KEY_K,
	KEY_SEMICOLON,
	KEY_BACKSLASH,
	KEY_COMMA,
	KEY_SLASH,
	KEY_N,
	KEY_M,
	KEY_PERIOD,
	KEY_GRAVE,
	KEY_KEYPAD_DECIMAL,
	KEY_KEYPAD_MULTIPLY,
	KEY_KEYPAD_PLUS,
	KEY_KEYPAD_CLEAR,
	KEY_KEYPAD_DIVIDE,
	KEY_KEYPAD_ENTER,
	KEY_KEYPAD_MINUS,
	KEY_KEYPAD_EQUALS,
	KEY_KEYPAD_0,
	KEY_KEYPAD_1,
	KEY_KEYPAD_2,
	KEY_KEYPAD_3,
	KEY_KEYPAD_4,
	KEY_KEYPAD_5,
	KEY_KEYPAD_6,
	KEY_KEYPAD_7,
	KEY_KEYPAD_8,
	KEY_KEYPAD_9,
	KEY_RETURN,
	KEY_TAB,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_ESCAPE,
	KEY_COMMAND,
	KEY_SHIFT,
	KEY_CAPS_LOCK,
	KEY_ALT,
	KEY_CONTROL,
	KEY_RIGHT_COMMAND,
	KEY_RIGHT_SHIFT,
	KEY_RIGHT_ALT,
	KEY_RIGHT_CONTROL,
	KEY_FUNCTION,
	KEY_F17,
	KEY_F18,
	KEY_F19,
	KEY_F20,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F3,
	KEY_F8,
	KEY_F9,
	KEY_F11,
	KEY_F13,
	KEY_F16,
	KEY_F14,
	KEY_F10,
	KEY_F12,
	KEY_F15,
	KEY_HELP,
	KEY_HOME,
	KEY_PAGE_UP,
	KEY_DELETE,
	KEY_F4,
	KEY_END,
	KEY_F2,
	KEY_PAGE_DOWN,
	KEY_F1,
	KEY_LEFT_ARROW,
	KEY_RIGHT_ARROW,
	KEY_DOWN_ARROW,
	KEY_UP_ARROW,
	KEY_ISO_SECTION,
	KEY_MISSION_CONTROL,
	KEY_DASHBOARD,

	// SPECIAL KEYCODES //
	KEY_VOLUME_UP,   // NOTE: KEY_VOLUME_UP must be first in this list
	KEY_VOLUME_DOWN,
	KEY_BRIGHTNESS_UP,
	KEY_BRIGHTNESS_DOWN,
	KEY_MUTE,
	KEY_LAUNCH_PANEL,
	KEY_PAUSE_PLAY,
	KEY_NEXT,
	KEY_PREVIOUS,
	KEY_FAST_FORWARD,
	KEY_REWIND,
	KEY_KEYBOARD_BACKLIGHT_UP,
	KEY_KEYBOARD_BACKLIGHT_DOWN,
	KEY_KEYBOARD_BACKLIGHT_TOGGLE,

	NUM_KEYS
};

char * key_string(enum Key key);

int keyboard_press(enum Key key);
int keyboard_release(enum Key key);
int keyboard_tap(enum Key key);
int keyboard_type(enum Key * keys, int length);    // taps keys in succession
int keyboard_combo(enum Key * keys, int length);   // holds down all keys and then releases them in the same order

#endif /* KEYBOARD_H */
