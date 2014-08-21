#define K_MODIFIER_LCTRL   (1 << 0)
#define K_MODIFIER_LSHIFT  (1 << 1)
#define K_MODIFIER_LALT    (1 << 2)
#define K_MODIFIER_LGUI    (1 << 3)
#define K_MODIFIER_RCTRL   (1 << 4)
#define K_MODIFIER_RSHIFT  (1 << 5)
#define K_MODIFIER_RALT    (1 << 6)
#define K_MODIFIER_RGUI    (1 << 7)

#define K_LED_NUMLOCK      (1 << 0)
#define K_LED_CAPSLOCK     (1 << 1)
#define K_LED_SCROLLLOCK   (1 << 2)
#define K_LED_KANA         (1 << 3)

#define K_RESERVED_NO_EVENT 0x00
#define K_ERROR_ROLL_OVER 0x01
#define K_POST_FAIL 0x02
#define K_ERROR_UNDEFINED 0x03
#define K_A_a 0x04
#define K_B_b 0x05
#define K_C_c 0x06
#define K_D_d 0x07
#define K_E_e 0x08
#define K_F_f 0x09
#define K_G_g 0x0A
#define K_H_h 0x0B
#define K_I_i 0x0C
#define K_J_j 0x0D
#define K_K_k 0x0E
#define K_L_l 0x0F
#define K_M_m 0x10
#define K_N_n 0x11
#define K_O_o 0x12
#define K_P_p 0x13
#define K_Q_q 0x14
#define K_R_r 0x15
#define K_S_s 0x16
#define K_T_t 0x17
#define K_U_u 0x18
#define K_V_v 0x19
#define K_W_w 0x1A
#define K_X_x 0x1B
#define K_Y_y 0x1C
#define K_Z_z 0x1D
#define K_1_BANG 0x1E
#define K_2_AT 0x1F
#define K_3_HASH 0x20
#define K_4_DOLLAR 0x21
#define K_5_PERCENT 0x22
#define K_6_CARET 0x23
#define K_7_AMPERSAND 0x24
#define K_8_ASTERISK 0x25
#define K_9_LPAREN 0x26
#define K_0_RPAREN 0x27
#define K_RETURN 0x28
#define K_ESCAPE 0x29
#define K_BACKSPACE 0x2A
#define K_TAB 0x2B
#define K_SPACE 0x2C
#define K_DASH_UNDERSCORE 0x2D
#define K_EQUAL_PLUS 0x2E
#define K_LBRACKET_LBRACE 0x2F
#define K_RBRACKET_RBRACE 0x30
#define K_BACKSLASH_PIPE 0x31
#define K_NON_US_HASH_TILDE 0x32
#define K_SEMICOLON_COLON 0x33
#define K_QUOTE_DOUBLEQUOTE 0x34
#define K_GRAVE_TILDE 0x35
#define K_COMMA_LANGLE 0x36
#define K_PERIOD_RANGLE 0x37
#define K_SLASH_QUESTION 0x38
#define K_CAPSLOCK 0x39
#define K_F1 0x3A
#define K_F2 0x3B
#define K_F3 0x3C
#define K_F4 0x3D
#define K_F5 0x3E
#define K_F6 0x3F
#define K_F7 0x40
#define K_F8 0x41
#define K_F9 0x42
#define K_F10 0x43
#define K_F11 0x44
#define K_F12 0x45
#define K_PRINTSCREEN_SYSRQ 0x46
#define K_SCROLLLOCK 0x47
#define K_PAUSE_BREAK 0x48
#define K_INSERT 0x49
#define K_HOME 0x4A
#define K_PAGEUP 0x4B
#define K_DELETE 0x4C
#define K_END 0x4D
#define K_PAGEDOWN 0x4E
#define K_RIGHT 0x4F
#define K_LEFT 0x50
#define K_DOWN 0x51
#define K_UP 0x52
#define K_PAD_NUMLOCK 0x53
#define K_PAD_DIVIDE 0x54
#define K_PAD_MULTIPLY 0x55
#define K_PAD_MINUS 0x56
#define K_PAD_PLUS 0x57
#define K_PAD_ENTER 0x58
#define K_PAD_1_END 0x59
#define K_PAD_2_DOWN 0x5A
#define K_PAD_3_PAGEDOWN 0x5B
#define K_PAD_4_LEFT 0x5C
#define K_PAD_5 0x5D
#define K_PAD_6_RIGHT 0x5E
#define K_PAD_7_HOME 0x5F
#define K_PAD_8_UP 0x60
#define K_PAD_9_PAGEUP 0x61
#define K_PAD_0_INSERT 0x62
#define K_PAD_PERIOD_DELETE 0x63
#define K_NON_US_BACKSLASH_PIPE 0x64
#define K_APPLICATION 0x65
#define K_POWER 0x66
#define K_PAD_EQUAL 0x67
#define K_F13 0x68
#define K_F14 0x69
#define K_F15 0x6A
#define K_F16 0x6B
#define K_F17 0x6C
#define K_F18 0x6D
#define K_F19 0x6E
#define K_F20 0x6F
#define K_F21 0x70
#define K_F22 0x71
#define K_F23 0x72
#define K_F24 0x73
#define K_EXECUTE 0x74
#define K_HELP 0x75
#define K_MENU 0x76
#define K_SELECT 0x77
#define K_STOP 0x78
#define K_AGAIN 0x79
#define K_UNDO 0x7A
#define K_CUT 0x7B
#define K_COPY 0x7C
#define K_PASTE 0x7D
#define K_FIND 0x7E
#define K_MUTE 0x7F
#define K_VOLUMEUP 0x80
#define K_VOLUMEDOWN 0x81
#define K_LOCKING_CAPSLOCK 0x82
#define K_LOCKING_NUMLOCK 0x83
#define K_LOCKING_SCROLLLOCK 0x84
#define K_PAD_COMMA 0x85
#define K_PAD_EQUAL_AS400 0x86
#define K_INTERNATIONAL1 0x87
#define K_INTERNATIONAL2 0x88
#define K_INTERNATIONAL3 0x89
#define K_INTERNATIONAL4 0x8A
#define K_INTERNATIONAL5 0x8B
#define K_INTERNATIONAL6 0x8C
#define K_INTERNATIONAL7 0x8D
#define K_INTERNATIONAL8 0x8E
#define K_INTERNATIONAL9 0x8F
#define K_LANG1 0x90
#define K_LANG2 0x91
#define K_LANG3 0x92
#define K_LANG4 0x93
#define K_LANG5 0x94
#define K_LANG6 0x95
#define K_LANG7 0x96
#define K_LANG8 0x97
#define K_LANG9 0x98
#define K_ALTERNATE_ERASE 0x99
#define K_SYSRQ_ATTENTION 0x9A
#define K_CANCEL 0x9B
#define K_CLEAR 0x9C
#define K_PRIOR 0x9D
#define K_RETURN2 0x9E
#define K_SEPARATOR 0x9F
#define K_OUT 0xA0
#define K_OPER 0xA1
#define K_CLEAR_AGAIN 0xA2
#define K_CRSEL_PROPS 0xA3
#define K_EXSEL 0xA4

#define K_PAD_00 0xB0
#define K_PAD_000 0xB1
#define K_PAD_THOUSANDS_SEPARATOR 0xB2
#define K_PAD_DECIMAL_SEPARATOR 0xB3
#define K_PAD_CURRENCY_UNIT 0xB4
#define K_PAD_CURRENCY_SUBUNIT 0xB5
#define K_PAD_LPAREN 0xB6
#define K_PAD_RPAREN 0xB7
#define K_PAD_LBRACE 0xB8
#define K_PAD_RBRACE 0xB9
#define K_PAD_TAB 0xBA
#define K_PAD_BACKSPACE 0xBB
#define K_PAD_A 0xBC
#define K_PAD_B 0xBD
#define K_PAD_C 0xBE
#define K_PAD_D 0xBF
#define K_PAD_E 0xC0
#define K_PAD_F 0xC1
#define K_PAD_XOR 0xC2
#define K_PAD_CARET 0xC3
#define K_PAD_PERCENT 0xC4
#define K_PAD_LANGLE 0xC5
#define K_PAD_RANGLE 0xC6
#define K_PAD_AMPERSAND 0xC7
#define K_PAD_DOUBLE_AMPERSAND 0xC8
#define K_PAD_PIPE 0xC9
#define K_PAD_DOUBLE_PIPE 0xCA
#define K_PAD_COLON 0xCB
#define K_PAD_HASH 0xCC
#define K_PAD_SPACE 0xCD
#define K_PAD_AT 0xCE
#define K_PAD_BANG 0xCF
#define K_PAD_MEMORY_STORE 0xD0
#define K_PAD_MEMORY_RECALL 0xD1
#define K_PAD_MEMORY_CLEAR 0xD2
#define K_PAD_MEMORY_ADD 0xD3
#define K_PAD_MEMORY_SUBTRACT 0xD4
#define K_PAD_MEMORY_MULTIPLY 0xD5
#define K_PAD_MEMORY_DIVIDE 0xD6
#define K_PAD_PLUS_MINUS 0xD7
#define K_PAD_CLEAR 0xD8
#define K_PAD_CLEAR_ENTRY 0xD9
#define K_PAD_BINARY 0xDA
#define K_PAD_OCTAL 0xDB
#define K_PAD_DECIMAL 0xDC
#define K_PAD_HEXADECIMAL 0xDD

#define K_LCTRL 0xE0
#define K_LSHIFT 0xE1
#define K_LALT 0xE2
#define K_LGUI 0xE3
#define K_RCTRL 0xE4
#define K_RSHIFT 0xE5
#define K_RALT 0xE6
#define K_RGUI 0xE7