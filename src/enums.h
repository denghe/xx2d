#pragma once
#include "pch.h"

namespace xx {

    // following enums reference from raylib.h

    enum class KbdKeys : int16_t {
        Null = 0,               // used for no key pressed
        // alphanumeric keys
        Apostrophe = 39,        // Key: '
        Comma = 44,             // Key: ,
        Minus = 45,             // Key: -
        Period = 46,            // Key: .
        Slash = 47,             // Key: /
        Zero = 48,              // Key: 0
        One = 49,               // Key: 1
        Two = 50,               // Key: 2
        Three = 51,             // Key: 3
        Four = 52,              // Key: 4
        Five = 53,              // Key: 5
        Six = 54,               // Key: 6
        Seven = 55,             // Key: 7
        Eight = 56,             // Key: 8
        Nine = 57,              // Key: 9
        Semicolon = 59,         // Key: ;
        Equal = 61,             // Key: =
        A = 65,                 // Key: A | a
        B = 66,                 // Key: B | b
        C = 67,                 // Key: C | c
        D = 68,                 // Key: D | d
        E = 69,                 // Key: E | e
        F = 70,                 // Key: F | f
        G = 71,                 // Key: G | g
        H = 72,                 // Key: H | h
        I = 73,                 // Key: I | i
        J = 74,                 // Key: J | j
        K = 75,                 // Key: K | k
        L = 76,                 // Key: L | l
        M = 77,                 // Key: M | m
        N = 78,                 // Key: N | n
        O = 79,                 // Key: O | o
        P = 80,                 // Key: P | p
        Q = 81,                 // Key: Q | q
        R = 82,                 // Key: R | r
        S = 83,                 // Key: S | s
        T = 84,                 // Key: T | t
        U = 85,                 // Key: U | u
        V = 86,                 // Key: V | v
        W = 87,                 // Key: W | w
        X = 88,                 // Key: X | x
        Y = 89,                 // Key: Y | y
        Z = 90,                 // Key: Z | z
        LeftBracket = 91,       // Key: [
        Backslash = 92,         // Key: '\'
        RightBracket = 93,      // Key: ]
        Grave = 96,             // Key: `
        // function keys
        Space = 32,             // Key: Space
        Escape = 256,           // Key: Esc
        Enter = 257,            // Key: Enter
        Tab = 258,              // Key: Tab
        Backspace = 259,        // Key: Backspace
        Insert = 260,           // Key: Ins
        Delete = 261,           // Key: Del
        Right = 262,            // Key: Cursor right
        Left = 263,             // Key: Cursor left
        Down = 264,             // Key: Cursor down
        Up = 265,               // Key: Cursor up
        PageUp = 266,           // Key: Page up
        PageDown = 267,         // Key: Page down
        Home = 268,             // Key: Home
        End = 269,              // Key: End
        CapsLock = 280,         // Key: Caps lock
        ScrollLock = 281,       // Key: Scroll down
        NumLock = 282,          // Key: Num lock
        PrintScreen = 283,      // Key: Print screen
        Pause = 284,            // Key: Pause
        F1 = 290,               // Key: F1
        F2 = 291,               // Key: F2
        F3 = 292,               // Key: F3
        F4 = 293,               // Key: F4
        F5 = 294,               // Key: F5
        F6 = 295,               // Key: F6
        F7 = 296,               // Key: F7
        F8 = 297,               // Key: F8
        F9 = 298,               // Key: F9
        F10 = 299,              // Key: F10
        F11 = 300,              // Key: F11
        F12 = 301,              // Key: F12
        LeftShift = 340,        // Key: Shift left
        LeftControl = 341,      // Key: Control left
        LeftAlt = 342,          // Key: Alt left
        LeftSuper = 343,        // Key: Super left
        RightShift = 344,       // Key: Shift right
        RightControl = 345,     // Key: Control right
        RightAlt = 346,         // Key: Alt right
        RightSuper = 347,       // Key: Super right
        KbMenu = 348,           // Key: KB menu
        // keypad keys
        Kp0 = 320,              // Key: Keypad 0
        Kp1 = 321,              // Key: Keypad 1
        Kp2 = 322,              // Key: Keypad 2
        Kp3 = 323,              // Key: Keypad 3
        Kp4 = 324,              // Key: Keypad 4
        Kp5 = 325,              // Key: Keypad 5
        Kp6 = 326,              // Key: Keypad 6
        Kp7 = 327,              // Key: Keypad 7
        Kp8 = 328,              // Key: Keypad 8
        Kp9 = 329,              // Key: Keypad 9
        KpDecimal = 330,        // Key: Keypad .
        KpDivide = 331,         // Key: Keypad /
        KpMultiply = 332,       // Key: Keypad *
        KpSubtract = 333,       // Key: Keypad -
        KpAdd = 334,            // Key: Keypad +
        KpEnter = 335,          // Key: Keypad Enter
        KpEqual = 336,          // Key: Keypad =
        // android key buttons
        Back = 4,               // Key: Android back button
        Menu = 82,              // Key: Android menu button
        VolumeUp = 24,          // Key: Android volume up button
        VolumeDown = 25         // Key: Android volume down button
    };

    enum class Mbtns : uint8_t {
        Left = 0,               // Mouse button left
        Right = 1,              // Mouse button right
        Middle = 2,             // Mouse button middle (pressed wheel)
        Side = 3,               // Mouse button side (advanced mouse device)
        Extra = 4,              // Mouse button extra (advanced mouse device)
        Forward = 5,            // Mouse button forward (advanced mouse device)
        Back = 6,               // Mouse button back (advanced mouse device)
    };

    enum class GpdBtns : uint8_t {
        Unknown = 0,            // Unknown button, just for error checking
        LeftFaceUp,             // Gamepad left DPAD up button
        LeftFaceRight,          // Gamepad left DPAD right button
        LeftFaceDown,           // Gamepad left DPAD down button
        LeftFaceLeft,           // Gamepad left DPAD left button
        RightFaceUp,            // Gamepad right button up (i.e. PS3: Triangle, Xbox: Y)
        RightFaceRight,         // Gamepad right button right (i.e. PS3: Square, Xbox: X)
        RightFaceDown,          // Gamepad right button down (i.e. PS3: Cross, Xbox: A)
        RightFaceLeft,          // Gamepad right button left (i.e. PS3: Circle, Xbox: B)
        LeftTrigger1,           // Gamepad top/back trigger left (first), it could be a trailing button
        LeftTrigger2,           // Gamepad top/back trigger left (second), it could be a trailing button
        RightTrigger1,          // Gamepad top/back trigger right (one), it could be a trailing button
        RightTrigger2,          // Gamepad top/back trigger right (second), it could be a trailing button
        MiddleLeft,             // Gamepad center buttons, left one (i.e. PS3: Select)
        Middle,                 // Gamepad center buttons, middle one (i.e. PS3: PS, Xbox: XBOX)
        MiddleRight,            // Gamepad center buttons, right one (i.e. PS3: Start)
        LeftThumb,              // Gamepad joystick pressed button left
        RightThumb              // Gamepad joystick pressed button right
    };

    enum class GpadAxiss : uint8_t {
        LeftX = 0,              // Gamepad left stick X axis
        LeftY = 1,              // Gamepad left stick Y axis
        RightX = 2,             // Gamepad right stick X axis
        RightY = 3,             // Gamepad right stick Y axis
        LeftTrigger = 4,        // Gamepad back trigger left, pressure level: [1..-1]
        RightTrigger = 5        // Gamepad back trigger right, pressure level: [1..-1]
    };

}
