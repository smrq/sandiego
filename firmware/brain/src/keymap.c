#include "defs.h"
#include "keymap.h"

const keymap_t PROGMEM keymap = {
	.layers = {
		KEYMAP_LAYER(
			F1,          F2, F3, F4, F5, F6,                  /**/         F7, F8, F9,           F10,           F11,             F12,
			ESC,         Q,  W,  E,  R,  T,  NULL,            /**/ NULL,   Y,  U,  I,            O,             P,               BACKSPACE,
			GRAVE_TILDE, A,  S,  D,  F,  G,  TAB,             /**/ RETURN, H,  J,  K,            L,             SEMICOLON_COLON, QUOTE_DOUBLE,
			PRINTSCREEN, Z,  X,  C,  V,  B,  INSERT,          /**/ DELETE, N,  M,  COMMA_LANGLE, PERIOD_RANGLE, SLASH_QUESTION,  DASH_UNDERSCORE,
			LAYER_SHIFT(1), LCTRL, LSHIFT, SPACE, LGUI, LALT, /**/ RALT, RGUI, SPACE, RSHIFT, RCTRL, LAYER_SHIFT(1)
		),
		KEYMAP_LAYER(
			F13,    F14,     F15,  F16,  F17,  F18,                  /**/           F19,    F20,      F21,    F22,    F23,       F24,
			DOLLAR, CARET,   7,    8,    9,    ASTERISK, NULL,       /**/ NULL,     LPAREN, RPAREN,   LBRACE, RBRACE, LBRACKET,  RBRACKET,
			HASH,   0,       4,    5,    6,    EQUAL,    NUMLOCK,    /**/ CAPSLOCK, LEFT,   DOWN,     UP,     RIGHT,  BANG,      AMPERSAND,
			AT,     PERCENT, 1,    2,    3,    PLUS,     SCROLLLOCK, /**/ PAUSE,    HOME,   PAGEDOWN, PAGEUP, END,    BACKSLASH, PIPE,
			NULL, LCTRL, LSHIFT, SPACE, LGUI, LALT,                  /**/ RALT, RGUI, SPACE, RSHIFT, RCTRL, NULL
		)
	}
};
