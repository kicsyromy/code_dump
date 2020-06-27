#pragma once

#include "voot_global.hh"
#include "events/voot_event.hh"

VT_BEGIN_NAMESPACE

enum class KeyCode
{
    UNKNOWN = 0,

    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,

    One = 30,
    Two = 31,
    Three = 32,
    Four = 33,
    Five = 34,
    Six = 35,
    Seven = 36,
    Eight = 37,
    Nine = 38,
    Zero = 39,

    Return = 40,
    Escape = 41,
    Backspace = 42,
    Tab = 43,
    Space = 44,

    Minus = 45,
    Equals = 46,
    Leftbracket = 47,
    Rightbracket = 48,
    Backslash = 49,
    Nonushash = 50,
    Semicolon = 51,
    Apostrophe = 52,
    Grave = 53,
    Comma = 54,
    Period = 55,
    Slash = 56,

    Capslock = 57,

    F1 = 58,
    F2 = 59,
    F3 = 60,
    F4 = 61,
    F5 = 62,
    F6 = 63,
    F7 = 64,
    F8 = 65,
    F9 = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,

    PrintScreen = 70,
    ScrollLock = 71,
    Pause = 72,
    Insert = 73,

    Home = 74,
    Pageup = 75,
    Delete = 76,
    End = 77,
    Pagedown = 78,
    Right = 79,
    Left = 80,
    Down = 81,
    Up = 82,

    Numlockclear = 83,

    KpDivide = 84,
    KpMultiply = 85,
    KpMinus = 86,
    KpPlus = 87,
    KpEnter = 88,
    KpOne = 89,
    KpTwo = 90,
    KpThree = 91,
    KpFour = 92,
    KpFive = 93,
    KpSix = 94,
    KpSeven = 95,
    KpEight = 96,
    KpNine = 97,
    KpZero = 98,
    KpPeriod = 99,

    NonUsBackslash = 100,
    Application = 101,
    Power = 102,

    KpEquals = 103,
    F13 = 104,
    F14 = 105,
    F15 = 106,
    F16 = 107,
    F17 = 108,
    F18 = 109,
    F19 = 110,
    F20 = 111,
    F21 = 112,
    F22 = 113,
    F23 = 114,
    F24 = 115,
    Execute = 116,
    Help = 117,
    Menu = 118,
    Select = 119,
    Stop = 120,
    Again = 121,
    Undo = 122,
    Cut = 123,
    Copy = 124,
    Paste = 125,
    Find = 126,
    Mute = 127,
    Volumeup = 128,
    Volumedown = 129,

    Kp_comma = 133,
    Kp_equalsas400 = 134,

    International1 = 135,
    International2 = 136,
    International3 = 137,
    International4 = 138,
    International5 = 139,
    International6 = 140,
    International7 = 141,
    International8 = 142,
    International9 = 143,

    Lang1 = 144,
    Lang2 = 145,
    Lang3 = 146,
    Lang4 = 147,
    Lang5 = 148,
    Lang6 = 149,
    Lang7 = 150,
    Lang8 = 151,
    Lang9 = 152,

    Alterase = 153,
    Sysreq = 154,
    Cancel = 155,
    Clear = 156,
    Prior = 157,
    Return2 = 158,
    Separator = 159,
    Out = 160,
    Oper = 161,
    Clearagain = 162,
    Crsel = 163,
    Exsel = 164,

    Kp_00 = 176,
    Kp_000 = 177,
    Thousandsseparator = 178,
    Decimalseparator = 179,
    Currencyunit = 180,
    Currencysubunit = 181,
    KpLeftParen = 182,
    KpRightParen = 183,
    KpLeftBrace = 184,
    KpRightBrace = 185,
    KpTab = 186,
    KpBackspace = 187,
    KpA = 188,
    KpB = 189,
    KpC = 190,
    KpD = 191,
    KpE = 192,
    KpF = 193,
    KpXor = 194,
    KpPower = 195,
    KpPercent = 196,
    KpLess = 197,
    KpGreater = 198,
    KpAmpersand = 199,
    KpDblAmpersand = 200,
    KpVerticalBar = 201,
    KpDblVerticalBar = 202,
    KpColon = 203,
    KpHash = 204,
    KpSpace = 205,
    KpAt = 206,
    KpExclam = 207,
    KpMemStore = 208,
    KpMemRecall = 209,
    KpMemClear = 210,
    KpMemAdd = 211,
    KpMemSubtract = 212,
    KpMemMultiply = 213,
    KpMemDivide = 214,
    KpPlusMinus = 215,
    KpClear = 216,
    KpClearEntry = 217,
    KpBinary = 218,
    KpOctal = 219,
    KpDecimal = 220,
    KpHexadecimal = 221,

    LCtrl = 224,
    LShift = 225,
    LAlt = 226,
    LGui = 227,
    RCtrl = 228,
    RShift = 229,
    RAlt = 230,
    RGui = 231,

    Mode = 257,

    AudioNext = 258,
    AudioPrev = 259,
    AudioStop = 260,
    AudioPlay = 261,
    AudioMute = 262,
    MediaSelect = 263,
    Www = 264,
    Mail = 265,
    Calculator = 266,
    Computer = 267,
    AcSearch = 268,
    AcHome = 269,
    AcBack = 270,
    AcForward = 271,
    AcStop = 272,
    AcRefresh = 273,
    AcBookmarks = 274,

    BrightnessDown = 275,
    BrightnessUp = 276,
    DisplaySwitch = 277,

    KbdIllumToggle = 278,
    KbdIllumDown = 279,
    KbdIllumUp = 280,
    Eject = 281,
    Sleep = 282,

    App1 = 283,
    App2 = 284,

    Audiorewind = 285,
    Audiofastforward = 286,

    Count = 512
};

constexpr std::size_t KEY_CODE_COUNT{ std::size_t(KeyCode::Count) };

class KeyEvent
{
public:
    static constexpr std::uint32_t EVENT_CATEGORIES{ EventCategoryInput | EventCategoryKeyboard };

public:
    constexpr KeyEvent(KeyCode key) noexcept
      : key_{ key }
    {}

public:
    constexpr KeyCode key() const
    {
        /* FIXME: Clear Linux's build of GCC complains about a potential null dereference *shrug* */
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
        return key_;
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
    }

protected:
    KeyCode key_{ KeyCode::UNKNOWN };
};

class KeyPressEvent
  : public KeyEvent
  , public EventBase<KeyPressEvent>
{
    VT_DECLARE_EVENT(KeyPressed, KeyEvent::EVENT_CATEGORIES);

public:
    constexpr KeyPressEvent(KeyCode key) noexcept
      : KeyEvent{ key }
      , EventBase<KeyPressEvent>{}
    {}
};

class KeyReleaseEvent
  : public KeyEvent
  , public EventBase<KeyReleaseEvent>
{
    VT_DECLARE_EVENT(KeyReleased, KeyEvent::EVENT_CATEGORIES);

public:
    constexpr KeyReleaseEvent(KeyCode key) noexcept
      : KeyEvent{ key }
      , EventBase<KeyReleaseEvent>{}
    {}
};

VT_END_NAMESPACE
