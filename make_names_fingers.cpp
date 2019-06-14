#include <iostream>
#include <string>
#include <fstream>

#include "heatmap.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Сначала нужно знать размер раскладки (взято из qmk/keyboards/ergodox_ez/config.h)
#define MATRIX_ROWS 14
#define MATRIX_COLS 6

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Здесь должен быть ваш макрос для раскладки
// Модифицируйте его так, чтобы было как здесь, с решетками, и чтобы KC_NO было строкой (взято из qmk/keyboards/ergodox_ez/ergodox_ez.h)

#define LAYOUT_ergodox(                                         \
                                                                \
    k00,k01,k02,k03,k04,k05,k06,                                \
    k10,k11,k12,k13,k14,k15,k16,                                \
    k20,k21,k22,k23,k24,k25,                                    \
    k30,k31,k32,k33,k34,k35,k36,                                \
    k40,k41,k42,k43,k44,                                        \
                            k55,k56,                            \
                                k54,                            \
                        k53,k52,k51,                            \
                                                                \
        k07,k08,k09,k0A,k0B,k0C,k0D,                            \
        k17,k18,k19,k1A,k1B,k1C,k1D,                            \
            k28,k29,k2A,k2B,k2C,k2D,                            \
        k37,k38,k39,k3A,k3B,k3C,k3D,                            \
                k49,k4A,k4B,k4C,k4D,                            \
    k57,k58,                                                    \
    k59,                                                        \
    k5C,k5B,k5A )                                               \
                                                                \
   /* matrix positions */                                       \
   {                                                            \
    { #k00, #k10, #k20, #k30, #k40, "KC_NO" },   \
    { #k01, #k11, #k21, #k31, #k41, #k51 },   \
    { #k02, #k12, #k22, #k32, #k42, #k52 },   \
    { #k03, #k13, #k23, #k33, #k43, #k53 },   \
    { #k04, #k14, #k24, #k34, #k44, #k54 },   \
    { #k05, #k15, #k25, #k35, "KC_NO", #k55 },   \
    { #k06, #k16, "KC_NO", #k36, "KC_NO", #k56 },   \
                                                                 \
    { #k07, #k17, "KC_NO", #k37,"KC_NO", #k57 },   \
    { #k08, #k18, #k28, #k38,"KC_NO", #k58 },   \
    { #k09, #k19, #k29, #k39, #k49, #k59 },   \
    { #k0A, #k1A, #k2A, #k3A, #k4A, #k5A },   \
    { #k0B, #k1B, #k2B, #k3B, #k4B, #k5B },   \
    { #k0C, #k1C, #k2C, #k3C, #k4C, #k5C },   \
    { #k0D, #k1D, #k2D, #k3D, #k4D, "KC_NO" }    \
   }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Здесь должна быть ваша раскладка из qmk, только тип надо заменить на char*

/*const char* PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    KC_ESC,     KC_AMPR,  KC_LBRC,  KC_RBRC,  KC_PLUS,  KC_EQL, CT_X,
    KC_TAB,     KC_SCLN,  KC_LCBR,  KC_RCBR,  KC_P,     KC_Y,   CT_C,
    MO(4),      KC_A,     KC_O,     KC_E,     KC_U,     KC_I,
    MO(5),      KC_QUOT,  KC_Q,     KC_J,     KC_K,     KC_X,   CT_V,
    TG(4),      TG(5),    TG(6),    CT_SLSH,  KC_SLSH,

        CT_A,     KC_DEL,   MY_CTRL,
        SHF_1,    KC_BSPC,  KC_ENT,


    SH_PSCR,    KC_ASTR,  KC_EXLM,  KC_RPRN,  KC_LPRN,  KC_QUES,  CT_Z,
    CT_D,       KC_F,     KC_G,     KC_C,     KC_R,     KC_L,     KC_BSLS,
                KC_D,     KC_H,     KC_T,     KC_N,     KC_S,     KC_MINS,
    CT_S,       KC_B,     KC_M,     KC_W,     KC_V,     KC_Z,     KC_HASH,
                          EN_CMSP,  KG_NEXT,  MY_SHAL,  MY_CTAL,  MY_MCAS,

        MY_CTSH,  MY_ALT,   MY_WIN,
        MY_LANG,  KC_DOT,   KC_SPC),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    _______,    IS_7,     IS_5,     IS_3,     IS_1,     IS_9,     IS_GRV,
    _______,    KC_COLN,  KC_LT,    KC_GT,    _______,  _______,  KC_DLR,
    _______,    _______,  _______,  _______,  _______,  _______,
    _______,    KC_DQT,   _______,  _______,  _______,  _______,  KC_TILD,
    _______,    _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  _______,  _______,


    _______,    IS_0,     IS_2,     IS_4,     IS_6,     IS_8,     KC_PERC,
    _______,    _______,  _______,  _______,  _______,  _______,  KC_PIPE,
                _______,  _______,  _______,  _______,  _______,  KC_AT,
    _______,    _______,  _______,  _______,  _______,  _______,  KC_CIRC,
                          IS_COMM,  _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  KC_UNDS,  _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    _______,    KC_PLUS,  KC_MINS,  KC_DLR,   KC_AT,    KC_CIRC,  CT_X,
    _______,    KC_Q,     KC_W,     KC_E,     KC_T,     KC_A,     CT_C,
    _______,    KC_R,     KC_V,     KC_D,     KC_F,     KC_G,
    _______,    KC_Z,     KC_X,     KC_C,     KC_B,     KC_S,     CT_V,
    _______,    _______,  _______,  _______,  KC_UNDS,

        _______,   _______,   _______,
        SHF_3,     _______,   _______,


    _______,    KC_ASTR,  KC_EXLM,  KC_RPRN,  KC_LPRN,  KC_AMPR,  CT_Z,
    _______,    KC_O,     KC_U,     KC_N,     KC_Y,     KC_P,     KC_RBRC,
                KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
    _______,    KC_I,     KC_M,     KC_COMM,  KC_DOT,   KC_LBRC,  KC_GRV,
                          RU_CMSP,  KG_NEXT,  _______,  _______,  _______,

        _______,   _______,   _______,
        _______,   RU_SDOT,   _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    _______,    IS_7,     IS_5,     IS_3,     IS_1,     IS_9,     _______,
    _______,    _______,  _______,  _______,  _______,  _______,  KC_HASH,
    _______,    _______,  _______,  _______,  _______,  _______,
    _______,    _______,  _______,  _______,  _______,  _______,  IS_EQL,
    _______,    _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  _______,  _______,


    _______,    IS_0,     IS_2,     IS_4,     IS_6,     IS_8,     KC_PERC,
    _______,    _______,  _______,  _______,  _______,  _______,  _______,
                _______,  _______,  _______,  _______,  _______,  _______,
    _______,    _______,  _______,  _______,  _______,  _______,  _______,
                          KC_QUES,  _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  IS_SLSH,  _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    TG(4),      KC_F7,    KC_F5,    KC_F3,    KC_F1,    KC_F9,    CT_X,
    AU_MUTE,    SH_F11,   CT_F10,   KC_F10,   KC_F11,   WN_D,     CT_C,
    _______,    AU_VOLU,  AU_NEXT,  CS_TAB,   CT_TAB,   AL_TAB,
    _______,    AU_VOLD,  AU_PREV,  CT_1,     CT_2,     AL_TTAB,  CT_V,
    DN_PLY2,    DN_STOP,  DN_STR2,  WN_E,     AU_PLAY,

        _______,  _______,  _______,
        _______,  _______,  _______,

    KC_NLCK,    KC_F10,   KC_F2,    KC_F4,    KC_F6,    KC_F8,    KC_F11,
    KC_INSERT,  CT_DEL,   KC_HOME,  KC_END,   KC_PGUP,  KC_PGDN,  KC_F12,
                CT_BSPC,  KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT,  KC_PSCR,
    KC_PAUSE,   CT_ENT,   CT_LEFT,  CT_UP,    CT_DOWN,  CT_RGHT,  AL_PSCR,
                          _______,   _______, DN_STR1,  DN_STOP,  DN_PLY1,

        _______,  _______,  _______,
        _______,  _______,  _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    TG(5),      CT_1,     CT_2,     CT_3,     CT_4,     CT_5,     CT_X,
    KC_F5,      KC_F6,    CS_T,     CT_T,     CT_W,     CT_J,     CT_C,
    KC_BTN2,    KC_BTN1,  KC_BTN3,  CS_TAB,   CT_TAB,   CT_D,
    _______,    CT_S,     KC_ENT,   KC_UP,    KC_DOWN,  CT_F,     CT_V,
    _______,    _______,  _______,  _______,  _______,  

        _______,  _______,  _______,
        _______,  _______,  _______,

    _______,    _______,  _______,  _______,  _______,  _______,  _______,
    _______,    _______,  _______,  _______,  _______,  _______,  _______,
                _______,  MS_LEFT,  MS_UP,    MS_DOWN,  MS_RGHT,  _______,
    _______,    _______,  MSWH_LF,  MSWH_UP,  MSWH_DW,  MSWH_RG,  _______,
                _______,  _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  _______,  _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    KC_ESC,     KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,
    KC_TAB,     KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_7,
    KC_LSFT,    KC_A,     KC_S,     KC_D,     KC_F,     KC_G,
    KC_LCTL,    KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_8,
    XXXXXXX,    XXXXXXX,  TG(6),    KC_PERC,  RU_CMSP,

        XXXXXXX,   XXXXXXX,   XXXXXXX,
        KC_SPC,    KC_LALT,   KC_ENT,


    XXXXXXX,    KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     XXXXXXX,
    XXXXXXX,    KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_RBRC,
                KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
    XXXXXXX,    KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_UNDS,
                          KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT,  XXXXXXX,

        XXXXXXX,   XXXXXXX,   XXXXXXX,
        XXXXXXX,   XXXXXXX,   XXXXXXX),
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Далее можно рисовать строки, как в следующей закомменченом примере
// Но тот вариант можно использовать только для начала, если вы не хотите настроить названия клавиш под себя. Тут можно все символы заменить на строки. Причем, если у вас есть символ запятая, конец скобки или ещё что-то, что может разрушить макрос, это можно заключить в кавычки:
// ",", "(", ")", "\"", "\\"
// Далее программа автоматически уберет кавычки, и все клавиши будут написаны нормально. Так же клавиша _______ автоматичеки будет преобразована в клавишу на слое ниже

const char* keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    Esc,        &,        "[",      "]",      +,        =,      Ctrl+X,
    Tab,        :,        "{",      "}",      p,        y,      Ctrl+C,
    MO(4),      a,        o,        e,        u,        i,
    MO(5),      "\"",     q,        j,        k,        x,      Ctrl+V,
    TG(4),      TG(5),    TG(6),    "Ctrl+/", "/",

        Ctrl+A,   Delete,   Ctrl,
        Shift,    Backspace,Enter,


    Shift+PrScr,*,        !,        ")",      "(",      ?,        Ctrl+Z,
    Ctrl+D,     f,        g,        c,        r,        l,        "\\",
                d,        h,        t,        n,        s,        -,
    Ctrl+S,     b,        m,        w,        v,        z,        "#",
                        ", ",Klavogonki Next, Shift+Alt,Ctrl+Alt, Ctrl+Alt+Shift,

        Ctrl+Shift,  Alt,   Win,
        Lang,        ".",   Space),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    _______,    7,        5,        3,        1,        9,        "`",
    _______,    :,        <,        >,        P,        Y,        $,
    _______,    A,        O,        E,        U,        I,      
    _______,    "'",      Q,        J,        K,        X,        ~,
    _______,    _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  _______,  _______,


    _______,    0,        2,        4,        6,        8,        %,
    _______,    F,        G,        C,        R,        L,        |,
                D,        H,        T,        N,        S,        @,
    _______,    B,        M,        W,        V,        Z,        ^,
                          ",",      _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  "_",      _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    _______,    +,        -,        ;,        "\"",     :,        _______,
    _______,    й,        ц,        у,        е,        ф,        _______,
    _______,    к,        м,        в,        а,        п,   
    _______,    я,        ч,        с,        и,        ы,        _______,
    _______,    _______,  _______,  _______,  _,

        _______,   _______,   _______,
        Shift,     _______,   _______,


    _______,    *,        !,        ")",     "(",       ?,        _______,
    _______,    щ,        г,        т,        н,        з,        ъ,
                р,        о,        л,        д,        ж,        э,
    _______,    ш,        ь,        б,        ю,        х,        ё,
                          ", ",     _______,  _______,  _______,  _______,

        _______,   _______,          _______,
        _______,   ". Auto Shift",   _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    _______,    7,        5,        3,        1,        9,        _______,
    _______,    Й,        Ц,        У,        Е,        Ф,        №,
    _______,    К,        М,        В,        А,        П,   
    _______,    Я,        Ч,        С,        И,        Ы,        =,
    _______,    _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  _______,  _______,


    _______,    0,        2,        4,        6,        8,        %,
    _______,    Щ,        Г,        Т,        Н,        З,        Ъ,
                Р,        О,        Л,        Д,        Ж,        Э,
    _______,    Ш,        Ь,        Б,        Ю,        Х,        Ё,
                          ",",  _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  ".",      _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    TG(4),      F7,       F5,       F3,       F1,       F9,       Ctrl+X,
    Mute sound,     Shift+F11,  Ctrl+F10, F10,      F11,      Win+D,    Ctrl+C,
    _______,    Volume up,  Next track,  Ctrl+Shift+Tab,   Ctrl+Tab,   Alt+Tab,
    _______,    Volume down,  Prev track,  Ctrl+1,     Ctrl+2,Alt+Tab+Tab, Ctrl+V,
    Dynamic macros play 2,    Dynamic macros stop recording,  Dynamic macros start recording 2,  Win+E,     Play music,

        _______,  _______,  _______,
        _______,  _______,  _______,

    Num Lock,    F10,   F2,    F4,    F6,    F8,    F11,
    Insert,  Ctrl+Delete,   Home,  End,   Page Up,  Page Down,  F12,
                Ctrl+Backspace,  Left,  Up,    Down,  Right,  Print Screen,
    Pause,   Ctrl+Enter,   Ctrl+Left,  Ctrl+Up,    Ctrl+Down,  Ctrl+Right,  Alt+Print screen,
                          _______,   _______, Dynamic macros start recording 1,  Dynamic macros stop recording,  Dynamic macros play 1,

        _______,  _______,  _______,
        _______,  _______,  _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    TG(5),      Ctrl+1,     Ctrl+2,     Ctrl+3,     Ctrl+4,     Ctrl+5,     Ctrl+X,
    F5,      F6,    Ctrl+Shift+T,     Ctrl+T,     Ctrl+W,     Ctrl+J,     Ctrl+C,
    Mouse Right Button,    Mouse Left Button,  Mouse Middle Button,  Ctrl+Shift+Tab,   Ctrl+Tab,   Ctrl+D,
    _______,    Ctrl+S,     Enter,   Up,    Down,  Ctrl+F,     Ctrl+V,
    _______,    _______,  _______,  _______,  _______,  

        _______,  _______,  _______,
        _______,  _______,  _______,

    _______,    _______,  _______,  _______,  _______,  _______,  _______,
    _______,    _______,  _______,  _______,  _______,  _______,  _______,
                _______,  Mouse Left 1px,  Mouse Up 1px,    Mouse Down 1px,  Mouse Right 1px,  _______,
    _______,    _______,  Mouse Wheel Left,  Mouse Wheel Up,  Mouse Wheel Down,  Mouse Wheel Up,  _______,
                _______,  _______,  _______,  _______,  _______,

        _______,  _______,  _______,
        _______,  _______,  _______),

  //---------------------------------------------------------------------------
  LAYOUT_ergodox(
    Esc,     1,     2,     3,     4,     5,     6,
    Tab,     q,     w,     e,     r,     t,     7,
    Shift,   a,     s,     d,     f,     g,
    Ctrl,    z,     x,     c,     v,     b,     8,
    XXXXXXX,    XXXXXXX,  TG(6),  %,     ", ",

        XXXXXXX,   XXXXXXX,   XXXXXXX,
        Space,    Alt,   Enter,


    XXXXXXX,    6,     7,     8,     9,     0,     XXXXXXX,
    XXXXXXX,    y,     u,     i,     o,     p,     ],
                h,     j,     k,     l,     ";",   "'",
    XXXXXXX,    n,     m,     ",",   ".",   "/",   _,
                       Left,  Up,    Down,  Right, XXXXXXX,

        XXXXXXX,   XXXXXXX,   XXXXXXX,
        XXXXXXX,   XXXXXXX,   XXXXXXX),
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Здесь должен быть ваш макрос для раскладки, только KC_NO должна быть заменена на 00

#define LAYOUT_ergodox1(                                        \
                                                                \
    k00,k01,k02,k03,k04,k05,k06,                                \
    k10,k11,k12,k13,k14,k15,k16,                                \
    k20,k21,k22,k23,k24,k25,                                    \
    k30,k31,k32,k33,k34,k35,k36,                                \
    k40,k41,k42,k43,k44,                                        \
                            k55,k56,                            \
                                k54,                            \
                        k53,k52,k51,                            \
                                                                \
        k07,k08,k09,k0A,k0B,k0C,k0D,                            \
        k17,k18,k19,k1A,k1B,k1C,k1D,                            \
            k28,k29,k2A,k2B,k2C,k2D,                            \
        k37,k38,k39,k3A,k3B,k3C,k3D,                            \
                k49,k4A,k4B,k4C,k4D,                            \
    k57,k58,                                                    \
    k59,                                                        \
    k5C,k5B,k5A )                                               \
                                                                \
   /* matrix positions */                                       \
   {                                                            \
    { k00, k10, k20, k30, k40, 0   },   \
    { k01, k11, k21, k31, k41, k51 },   \
    { k02, k12, k22, k32, k42, k52 },   \
    { k03, k13, k23, k33, k43, k53 },   \
    { k04, k14, k24, k34, k44, k54 },   \
    { k05, k15, k25, k35, 0,   k55 },   \
    { k06, k16, 0,   k36, 0,   k56 },   \
                                                                 \
    { k07, k17, 0,   k37, 0,   k57 },   \
    { k08, k18, k28, k38, 0,   k58 },   \
    { k09, k19, k29, k39, k49, k59 },   \
    { k0A, k1A, k2A, k3A, k4A, k5A },   \
    { k0B, k1B, k2B, k3B, k4B, k5B },   \
    { k0C, k1C, k2C, k3C, k4C, k5C },   \
    { k0D, k1D, k2D, k3D, k4D, 0   }    \
   }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/* 
    Здесь должна быть ваша раскладка из qmk, только для каждой клавиши надо задать палец и ряд. Виды пальцев: 
        0 - левый мизинец; 
        1 - левый безымянный; 
        2 - левый средний; 
        3 - левый указательный; 
        4 - левый большой; 
        5 - правый большой; 
        6 - правый указательный; 
        7 - правый средний; 
        8 - правый безымянный; 
        9 - правый мизинец. 
    Ряды: 
        1 - нижний на две позиции; 
        2 - нижний на одну позицию; 
        3 - домашний; 
        4 - верхний на одну позицию; 
        5 - верхний на две позиции (там обычно находятся цифры). 
    Сначала задается ряд, потом палец, например: 32 - домашний ряд, левый безымянный палец. Рекомендуется не использовать комбинации 30, 02 и т. д., а только одну: 00 - это означает любой палец, нажимающийся не из домашней позиции.
*/

// Необходимо её назвать keymaps2

const uint16_t keymaps2[MATRIX_ROWS][MATRIX_COLS] =
  LAYOUT_ergodox1(
    00,         50,       51,       52,       53,       53,     53,
    40,         40,       41,       42,       43,       43,     43,
    30,         30,       31,       32,       33,       33,
    20,         20,       21,       22,       23,       23,     23,
    00,         00,       00,       14,       24,

        00,       00,       54,
        34,       44,       54,


    56,         56,       56,       57,       58,       59,       00,
    46,         46,       46,       47,       48,       49,       49,
                36,       36,       37,       38,       39,       39,
    26,         26,       26,       27,       28,       29,       29,
                          25,       15,       00,       00,       00,

        00,       00,       55,
        55,       45,       35);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string getNormalString(int layer, int row, int col) {
	std::string result;
	start:
	result = keymaps[layer][row][col];
	if (result.back() == result.front() && result.front() == '"') {
		result.pop_back();
		result.erase(result.begin());
	}
	if (result == "_______") {
		if (layer != 0) {
			layer--;
			goto start;
		}
	}
	if (result == "\\\\")
		result = "\\";
	if (result == "\\\"")
		result = "\"";
	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	Names names;
	int layersCount = sizeof(keymaps)/(sizeof(char*)*MATRIX_COLS*MATRIX_ROWS);
	for (int i = 0; i < layersCount; ++i) {
		for (int j = 0; j < MATRIX_ROWS; ++j) {
			for (int k = 0; k < MATRIX_COLS; ++k) {
				names.setName({k, j, i}, getNormalString(i, j, k));
			}
		}
	}
	std::ofstream fout("layout_names.txt");
	names.save(fout);
	fout.close();

	Fingers fingers;
	for (int j = 0; j < MATRIX_ROWS; ++j) {
		for (int k = 0; k < MATRIX_COLS; ++k) {
			auto value = keymaps2[j][k];
			fingers.set({k, j, 0}, value%10, (value/10)%10);
		}
	}
	fout.open("layout_fingers.txt");
	fingers.save(fout);
	fout.close();
}