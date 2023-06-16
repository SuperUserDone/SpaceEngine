#pragma once

// Stolen from SDL

#include <array>

namespace input {
namespace keyboard {

enum key : size_t {
  KEY_UNKNOWN = 0,

  /**
   *  \name Usage page 0x07
   *
   *  These values are from usage page 0x07 (USB keyboard page).
   */
  /* @{ */

  KEY_A = 4,
  KEY_B = 5,
  KEY_C = 6,
  KEY_D = 7,
  KEY_E = 8,
  KEY_F = 9,
  KEY_G = 10,
  KEY_H = 11,
  KEY_I = 12,
  KEY_J = 13,
  KEY_K = 14,
  KEY_L = 15,
  KEY_M = 16,
  KEY_N = 17,
  KEY_O = 18,
  KEY_P = 19,
  KEY_Q = 20,
  KEY_R = 21,
  KEY_S = 22,
  KEY_T = 23,
  KEY_U = 24,
  KEY_V = 25,
  KEY_W = 26,
  KEY_X = 27,
  KEY_Y = 28,
  KEY_Z = 29,

  KEY_1 = 30,
  KEY_2 = 31,
  KEY_3 = 32,
  KEY_4 = 33,
  KEY_5 = 34,
  KEY_6 = 35,
  KEY_7 = 36,
  KEY_8 = 37,
  KEY_9 = 38,
  KEY_0 = 39,

  KEY_RETURN = 40,
  KEY_ESCAPE = 41,
  KEY_BACKSPACE = 42,
  KEY_TAB = 43,
  KEY_SPACE = 44,

  KEY_MINUS = 45,
  KEY_EQUALS = 46,
  KEY_LEFTBRACKET = 47,
  KEY_RIGHTBRACKET = 48,
  KEY_BACKSLASH = 49, /**< Located at the lower left of the return
                       *   key on ISO keyboards and at the right end
                       *   of the QWERTY row on ANSI keyboards.
                       *   Produces REVERSE SOLIDUS (backslash) and
                       *   VERTICAL LINE in a US layout, REVERSE
                       *   SOLIDUS and VERTICAL LINE in a UK Mac
                       *   layout, NUMBER SIGN and TILDE in a UK
                       *   Windows layout, DOLLAR SIGN and POUND SIGN
                       *   in a Swiss German layout, NUMBER SIGN and
                       *   APOSTROPHE in a German layout, GRAVE
                       *   ACCENT and POUND SIGN in a French Mac
                       *   layout, and ASTERISK and MICRO SIGN in a
                       *   French Windows layout.
                       */
  KEY_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                       *   instead of 49 for the same key, but all
                       *   OSes I've seen treat the two codes
                       *   identically. So, as an implementor, unless
                       *   your keyboard generates both of those
                       *   codes and your OS treats them differently,
                       *   you should generate KEY_BACKSLASH
                       *   instead of this code. As a user, you
                       *   should not rely on this code because SDL
                       *   will never generate it with most (all?)
                       *   keyboards.
                       */
  KEY_SEMICOLON = 51,
  KEY_APOSTROPHE = 52,
  KEY_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                   *   and ISO keyboards). Produces GRAVE ACCENT and
                   *   TILDE in a US Windows layout and in US and UK
                   *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                   *   and NOT SIGN in a UK Windows layout, SECTION
                   *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                   *   layouts on ISO keyboards, SECTION SIGN and
                   *   DEGREE SIGN in a Swiss German layout (Mac:
                   *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                   *   DEGREE SIGN in a German layout (Mac: only on
                   *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                   *   French Windows layout, COMMERCIAL AT and
                   *   NUMBER SIGN in a French Mac layout on ISO
                   *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                   *   SIGN in a Swiss German, German, or French Mac
                   *   layout on ANSI keyboards.
                   */
  KEY_COMMA = 54,
  KEY_PERIOD = 55,
  KEY_SLASH = 56,

  KEY_CAPSLOCK = 57,

  KEY_F1 = 58,
  KEY_F2 = 59,
  KEY_F3 = 60,
  KEY_F4 = 61,
  KEY_F5 = 62,
  KEY_F6 = 63,
  KEY_F7 = 64,
  KEY_F8 = 65,
  KEY_F9 = 66,
  KEY_F10 = 67,
  KEY_F11 = 68,
  KEY_F12 = 69,

  KEY_PRINTSCREEN = 70,
  KEY_SCROLLLOCK = 71,
  KEY_PAUSE = 72,
  KEY_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                 does send code 73, not 117) */
  KEY_HOME = 74,
  KEY_PAGEUP = 75,
  KEY_DELETE = 76,
  KEY_END = 77,
  KEY_PAGEDOWN = 78,
  KEY_RIGHT = 79,
  KEY_LEFT = 80,
  KEY_DOWN = 81,
  KEY_UP = 82,

  KEY_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                          */
  KEY_KP_DIVIDE = 84,
  KEY_KP_MULTIPLY = 85,
  KEY_KP_MINUS = 86,
  KEY_KP_PLUS = 87,
  KEY_KP_ENTER = 88,
  KEY_KP_1 = 89,
  KEY_KP_2 = 90,
  KEY_KP_3 = 91,
  KEY_KP_4 = 92,
  KEY_KP_5 = 93,
  KEY_KP_6 = 94,
  KEY_KP_7 = 95,
  KEY_KP_8 = 96,
  KEY_KP_9 = 97,
  KEY_KP_0 = 98,
  KEY_KP_PERIOD = 99,

  KEY_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                             *   keyboards have over ANSI ones,
                             *   located between left shift and Y.
                             *   Produces GRAVE ACCENT and TILDE in a
                             *   US or UK Mac layout, REVERSE SOLIDUS
                             *   (backslash) and VERTICAL LINE in a
                             *   US or UK Windows layout, and
                             *   LESS-THAN SIGN and GREATER-THAN SIGN
                             *   in a Swiss German, German, or French
                             *   layout. */
  KEY_APPLICATION = 101,    /**< windows contextual menu, compose */
  KEY_POWER = 102,          /**< The USB document says this is a status flag,
                             *   not a physical key - but some Mac keyboards
                             *   do have a power key. */
  KEY_KP_EQUALS = 103,
  KEY_F13 = 104,
  KEY_F14 = 105,
  KEY_F15 = 106,
  KEY_F16 = 107,
  KEY_F17 = 108,
  KEY_F18 = 109,
  KEY_F19 = 110,
  KEY_F20 = 111,
  KEY_F21 = 112,
  KEY_F22 = 113,
  KEY_F23 = 114,
  KEY_F24 = 115,
  KEY_EXECUTE = 116,
  KEY_HELP = 117,  /**< AL Integrated Help Center */
  KEY_MENU = 118,  /**< Menu (show menu) */
  KEY_SELECT = 119,
  KEY_STOP = 120,  /**< AC Stop */
  KEY_AGAIN = 121, /**< AC Redo/Repeat */
  KEY_UNDO = 122,  /**< AC Undo */
  KEY_CUT = 123,   /**< AC Cut */
  KEY_COPY = 124,  /**< AC Copy */
  KEY_PASTE = 125, /**< AC Paste */
  KEY_FIND = 126,  /**< AC Find */
  KEY_MUTE = 127,
  KEY_VOLUMEUP = 128,
  KEY_VOLUMEDOWN = 129,
  /* not sure whether there's a reason to enable these */
  /*     KEY_LOCKINGCAPSLOCK = 130,  */
  /*     KEY_LOCKINGNUMLOCK = 131, */
  /*     KEY_LOCKINGSCROLLLOCK = 132, */
  KEY_KP_COMMA = 133,
  KEY_KP_EQUALSAS400 = 134,

  KEY_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                          footnotes in USB doc */
  KEY_INTERNATIONAL2 = 136,
  KEY_INTERNATIONAL3 = 137, /**< Yen */
  KEY_INTERNATIONAL4 = 138,
  KEY_INTERNATIONAL5 = 139,
  KEY_INTERNATIONAL6 = 140,
  KEY_INTERNATIONAL7 = 141,
  KEY_INTERNATIONAL8 = 142,
  KEY_INTERNATIONAL9 = 143,
  KEY_LANG1 = 144,    /**< Hangul/English toggle */
  KEY_LANG2 = 145,    /**< Hanja conversion */
  KEY_LANG3 = 146,    /**< Katakana */
  KEY_LANG4 = 147,    /**< Hiragana */
  KEY_LANG5 = 148,    /**< Zenkaku/Hankaku */
  KEY_LANG6 = 149,    /**< reserved */
  KEY_LANG7 = 150,    /**< reserved */
  KEY_LANG8 = 151,    /**< reserved */
  KEY_LANG9 = 152,    /**< reserved */

  KEY_ALTERASE = 153, /**< Erase-Eaze */
  KEY_SYSREQ = 154,
  KEY_CANCEL = 155,   /**< AC Cancel */
  KEY_CLEAR = 156,
  KEY_PRIOR = 157,
  KEY_RETURN2 = 158,
  KEY_SEPARATOR = 159,
  KEY_OUT = 160,
  KEY_OPER = 161,
  KEY_CLEARAGAIN = 162,
  KEY_CRSEL = 163,
  KEY_EXSEL = 164,

  KEY_KP_00 = 176,
  KEY_KP_000 = 177,
  KEY_THOUSANDSSEPARATOR = 178,
  KEY_DECIMALSEPARATOR = 179,
  KEY_CURRENCYUNIT = 180,
  KEY_CURRENCYSUBUNIT = 181,
  KEY_KP_LEFTPAREN = 182,
  KEY_KP_RIGHTPAREN = 183,
  KEY_KP_LEFTBRACE = 184,
  KEY_KP_RIGHTBRACE = 185,
  KEY_KP_TAB = 186,
  KEY_KP_BACKSPACE = 187,
  KEY_KP_A = 188,
  KEY_KP_B = 189,
  KEY_KP_C = 190,
  KEY_KP_D = 191,
  KEY_KP_E = 192,
  KEY_KP_F = 193,
  KEY_KP_XOR = 194,
  KEY_KP_POWER = 195,
  KEY_KP_PERCENT = 196,
  KEY_KP_LESS = 197,
  KEY_KP_GREATER = 198,
  KEY_KP_AMPERSAND = 199,
  KEY_KP_DBLAMPERSAND = 200,
  KEY_KP_VERTICALBAR = 201,
  KEY_KP_DBLVERTICALBAR = 202,
  KEY_KP_COLON = 203,
  KEY_KP_HASH = 204,
  KEY_KP_SPACE = 205,
  KEY_KP_AT = 206,
  KEY_KP_EXCLAM = 207,
  KEY_KP_MEMSTORE = 208,
  KEY_KP_MEMRECALL = 209,
  KEY_KP_MEMCLEAR = 210,
  KEY_KP_MEMADD = 211,
  KEY_KP_MEMSUBTRACT = 212,
  KEY_KP_MEMMULTIPLY = 213,
  KEY_KP_MEMDIVIDE = 214,
  KEY_KP_PLUSMINUS = 215,
  KEY_KP_CLEAR = 216,
  KEY_KP_CLEARENTRY = 217,
  KEY_KP_BINARY = 218,
  KEY_KP_OCTAL = 219,
  KEY_KP_DECIMAL = 220,
  KEY_KP_HEXADECIMAL = 221,

  KEY_LCTRL = 224,
  KEY_LSHIFT = 225,
  KEY_LALT = 226, /**< alt, option */
  KEY_LGUI = 227, /**< windows, command (apple), meta */
  KEY_RCTRL = 228,
  KEY_RSHIFT = 229,
  KEY_RALT = 230, /**< alt gr, option */
  KEY_RGUI = 231, /**< windows, command (apple), meta */

  KEY_MODE = 257, /**< I'm not sure if this is really not covered
                   *   by any of the above, but since there's a
                   *   special KMOD_MODE for it I'm adding it here
                   */

  /* @} */        /* Usage page 0x07 */

  /**
   *  \name Usage page 0x0C
   *
   *  These values are mapped from usage page 0x0C (USB consumer page).
   *  See https://usb.org/sites/default/files/hut1_2.pdf
   *
   *  There are way more keys in the spec than we can represent in the
   *  current scancode range, so pick the ones that commonly come up in
   *  real world usage.
   */
  /* @{ */

  KEY_AUDIONEXT = 258,
  KEY_AUDIOPREV = 259,
  KEY_AUDIOSTOP = 260,
  KEY_AUDIOPLAY = 261,
  KEY_AUDIOMUTE = 262,
  KEY_MEDIASELECT = 263,
  KEY_WWW = 264,          /**< AL Internet Browser */
  KEY_MAIL = 265,
  KEY_CALCULATOR = 266,   /**< AL Calculator */
  KEY_COMPUTER = 267,
  KEY_AC_SEARCH = 268,    /**< AC Search */
  KEY_AC_HOME = 269,      /**< AC Home */
  KEY_AC_BACK = 270,      /**< AC Back */
  KEY_AC_FORWARD = 271,   /**< AC Forward */
  KEY_AC_STOP = 272,      /**< AC Stop */
  KEY_AC_REFRESH = 273,   /**< AC Refresh */
  KEY_AC_BOOKMARKS = 274, /**< AC Bookmarks */

  /* @} */                /* Usage page 0x0C */

  /**
   *  \name Walther keys
   *
   *  These are values that Christian Walther added (for mac keyboard?).
   */
  /* @{ */

  KEY_BRIGHTNESSDOWN = 275,
  KEY_BRIGHTNESSUP = 276,
  KEY_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                         switch, video mode switch */
  KEY_KBDILLUMTOGGLE = 278,
  KEY_KBDILLUMDOWN = 279,
  KEY_KBDILLUMUP = 280,
  KEY_EJECT = 281,
  KEY_SLEEP = 282, /**< SC System Sleep */

  KEY_APP1 = 283,
  KEY_APP2 = 284,

  /* @} */ /* Walther keys */

  /**
   *  \name Usage page 0x0C (additional media keys)
   *
   *  These values are mapped from usage page 0x0C (USB consumer page).
   */
  /* @{ */

  KEY_AUDIOREWIND = 285,
  KEY_AUDIOFASTFORWARD = 286,

  /* @} */ /* Usage page 0x0C (additional media keys) */

  /**
   *  \name Mobile keys
   *
   *  These are values that are often used on mobile phones.
   */
  /* @{ */

  KEY_SOFTLEFT = 287,  /**< Usually situated below the display on phones and
                                     used as a multi-function feature key for selecting
                                     a software defined function shown on the bottom left
                                     of the display. */
  KEY_SOFTRIGHT = 288, /**< Usually situated below the display on phones and
                                     used as a multi-function feature key for selecting
                                     a software defined function shown on the bottom right
                                     of the display. */
  KEY_CALL = 289,      /**< Used for accepting phone calls. */
  KEY_ENDCALL = 290,   /**< Used for rejecting phone calls. */

  /* @} */             /* Mobile keys */

  /* Add any other keys here. */

  COUNT = 512 /**< not a key, just marks the number of scancodes
                               for array bounds */
};

constexpr auto key_names{[]() constexpr {
  std::array<const char *, COUNT> result{};
  for (int i = 0; i < COUNT; i++) {
    result[i] = "key.unknown";
  }

  result[4] = "key.a";
  result[5] = "key.b";
  result[6] = "key.c";
  result[7] = "key.d";
  result[8] = "key.e";
  result[9] = "key.f";
  result[10] = "key.g";
  result[11] = "key.h";
  result[12] = "key.i";
  result[13] = "key.j";
  result[14] = "key.k";
  result[15] = "key.l";
  result[16] = "key.m";
  result[17] = "key.n";
  result[18] = "key.o";
  result[19] = "key.p";
  result[20] = "key.q";
  result[21] = "key.r";
  result[22] = "key.s";
  result[23] = "key.t";
  result[24] = "key.u";
  result[25] = "key.v";
  result[26] = "key.w";
  result[27] = "key.x";
  result[28] = "key.y";
  result[29] = "key.z";
  result[30] = "key.1";
  result[31] = "key.2";
  result[32] = "key.3";
  result[33] = "key.4";
  result[34] = "key.5";
  result[35] = "key.6";
  result[36] = "key.7";
  result[37] = "key.8";
  result[38] = "key.9";
  result[39] = "key.0";
  result[40] = "key.return";
  result[41] = "key.escape";
  result[42] = "key.backspace";
  result[43] = "key.tab";
  result[44] = "key.space";
  result[45] = "key.minus";
  result[46] = "key.equals";
  result[47] = "key.leftbracket";
  result[48] = "key.rightbracket";
  result[49] = "key.backslash";
  result[50] = "key.nonushash";
  result[51] = "key.semicolon";
  result[52] = "key.apostrophe";
  result[53] = "key.grave";
  result[54] = "key.comma";
  result[55] = "key.period";
  result[56] = "key.slash";
  result[57] = "key.capslock";
  result[58] = "key.f1";
  result[59] = "key.f2";
  result[60] = "key.f3";
  result[61] = "key.f4";
  result[62] = "key.f5";
  result[63] = "key.f6";
  result[64] = "key.f7";
  result[65] = "key.f8";
  result[66] = "key.f9";
  result[67] = "key.f10";
  result[68] = "key.f11";
  result[69] = "key.f12";
  result[70] = "key.printscreen";
  result[71] = "key.scrolllock";
  result[72] = "key.pause";
  result[73] = "key.insert";
  result[74] = "key.home";
  result[75] = "key.pageup";
  result[76] = "key.delete";
  result[77] = "key.end";
  result[78] = "key.pagedown";
  result[79] = "key.right";
  result[80] = "key.left";
  result[81] = "key.down";
  result[82] = "key.up";
  result[83] = "key.numlockclear";
  result[84] = "key.kp_divide";
  result[85] = "key.kp_multiply";
  result[86] = "key.kp_minus";
  result[87] = "key.kp_plus";
  result[88] = "key.kp_enter";
  result[89] = "key.kp_1";
  result[90] = "key.kp_2";
  result[91] = "key.kp_3";
  result[92] = "key.kp_4";
  result[93] = "key.kp_5";
  result[94] = "key.kp_6";
  result[95] = "key.kp_7";
  result[96] = "key.kp_8";
  result[97] = "key.kp_9";
  result[98] = "key.kp_0";
  result[99] = "key.kp_period";
  result[100] = "key.nonusbackslash";
  result[101] = "key.application";
  result[102] = "key.power";
  result[103] = "key.kp_equals";
  result[104] = "key.f13";
  result[105] = "key.f14";
  result[106] = "key.f15";
  result[107] = "key.f16";
  result[108] = "key.f17";
  result[109] = "key.f18";
  result[110] = "key.f19";
  result[111] = "key.f20";
  result[112] = "key.f21";
  result[113] = "key.f22";
  result[114] = "key.f23";
  result[115] = "key.f24";
  result[116] = "key.execute";
  result[117] = "key.help";
  result[118] = "key.menu";
  result[119] = "key.select";
  result[120] = "key.stop";
  result[121] = "key.again";
  result[122] = "key.undo";
  result[123] = "key.cut";
  result[124] = "key.copy";
  result[125] = "key.paste";
  result[126] = "key.find";
  result[127] = "key.mute";
  result[128] = "key.volumeup";
  result[129] = "key.volumedown";
  result[133] = "key.kp_comma";
  result[134] = "key.kp_equalsas400";
  result[135] = "key.international1";
  result[136] = "key.international2";
  result[137] = "key.international3";
  result[138] = "key.international4";
  result[139] = "key.international5";
  result[140] = "key.international6";
  result[141] = "key.international7";
  result[142] = "key.international8";
  result[143] = "key.international9";
  result[144] = "key.lang1";
  result[145] = "key.lang2";
  result[146] = "key.lang3";
  result[147] = "key.lang4";
  result[148] = "key.lang5";
  result[149] = "key.lang6";
  result[150] = "key.lang7";
  result[151] = "key.lang8";
  result[152] = "key.lang9";
  result[153] = "key.alterase";
  result[154] = "key.sysreq";
  result[155] = "key.cancel";
  result[156] = "key.clear";
  result[157] = "key.prior";
  result[158] = "key.return2";
  result[159] = "key.separator";
  result[160] = "key.out";
  result[161] = "key.oper";
  result[162] = "key.clearagain";
  result[163] = "key.crsel";
  result[164] = "key.exsel";
  result[176] = "key.kp_00";
  result[177] = "key.kp_000";
  result[178] = "key.thousandsseparator";
  result[179] = "key.decimalseparator";
  result[180] = "key.currencyunit";
  result[181] = "key.currencysubunit";
  result[182] = "key.kp_leftparen";
  result[183] = "key.kp_rightparen";
  result[184] = "key.kp_leftbrace";
  result[185] = "key.kp_rightbrace";
  result[186] = "key.kp_tab";
  result[187] = "key.kp_backspace";
  result[188] = "key.kp_a";
  result[189] = "key.kp_b";
  result[190] = "key.kp_c";
  result[191] = "key.kp_d";
  result[192] = "key.kp_e";
  result[193] = "key.kp_f";
  result[194] = "key.kp_xor";
  result[195] = "key.kp_power";
  result[196] = "key.kp_percent";
  result[197] = "key.kp_less";
  result[198] = "key.kp_greater";
  result[199] = "key.kp_ampersand";
  result[200] = "key.kp_dblampersand";
  result[201] = "key.kp_verticalbar";
  result[202] = "key.kp_dblverticalbar";
  result[203] = "key.kp_colon";
  result[204] = "key.kp_hash";
  result[205] = "key.kp_space";
  result[206] = "key.kp_at";
  result[207] = "key.kp_exclam";
  result[208] = "key.kp_memstore";
  result[209] = "key.kp_memrecall";
  result[210] = "key.kp_memclear";
  result[211] = "key.kp_memadd";
  result[212] = "key.kp_memsubtract";
  result[213] = "key.kp_memmultiply";
  result[214] = "key.kp_memdivide";
  result[215] = "key.kp_plusminus";
  result[216] = "key.kp_clear";
  result[217] = "key.kp_clearentry";
  result[218] = "key.kp_binary";
  result[219] = "key.kp_octal";
  result[220] = "key.kp_decimal";
  result[221] = "key.kp_hexadecimal";
  result[224] = "key.lctrl";
  result[225] = "key.lshift";
  result[226] = "key.lalt";
  result[227] = "key.lgui";
  result[228] = "key.rctrl";
  result[229] = "key.rshift";
  result[230] = "key.ralt";
  result[231] = "key.rgui";
  result[257] = "key.mode";
  result[258] = "key.audionext";
  result[259] = "key.audioprev";
  result[260] = "key.audiostop";
  result[261] = "key.audioplay";
  result[262] = "key.audiomute";
  result[263] = "key.mediaselect";
  result[264] = "key.www";
  result[265] = "key.mail";
  result[266] = "key.calculator";
  result[267] = "key.computer";
  result[268] = "key.ac_search";
  result[269] = "key.ac_home";
  result[270] = "key.ac_back";
  result[271] = "key.ac_forward";
  result[272] = "key.ac_stop";
  result[273] = "key.ac_refresh";
  result[274] = "key.ac_bookmarks";
  result[275] = "key.brightnessdown";
  result[276] = "key.brightnessup";
  result[277] = "key.displayswitch";
  result[278] = "key.kbdillumtoggle";
  result[279] = "key.kbdillumdown";
  result[280] = "key.kbdillumup";
  result[281] = "key.eject";
  result[282] = "key.sleep";
  result[283] = "key.app1";
  result[284] = "key.app2";
  result[285] = "key.audiorewind";
  result[286] = "key.audiofastforward";
  result[287] = "key.softleft";
  result[288] = "key.softright";
  result[289] = "key.call";
  result[290] = "key.endcall";

  return result;
}()};

} // namespace keyboard
} // namespace input