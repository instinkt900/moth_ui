#pragma once

#include "event.h"

enum class KeyAction {
    Down,
    Up
};

enum class Key {
    Unknown,

    Return,
    Escape,
    Backspace,
    Tab,
    Space,
    Exclaim,
    Quotedbl,
    Hash,
    Percent,
    Dollar,
    Ampersand,
    Quote,
    Leftparen,
    Rightparen,
    Asterisk,
    Plus,
    Comma,
    Minus,
    Period,
    Slash,
    N0,
    N1,
    N2,
    N3,
    N4,
    N5,
    N6,
    N7,
    N8,
    N9,
    Colon,
    Semicolon,
    Less,
    Equals,
    Greater,
    Question,
    At,

    Leftbracket,
    Backslash,
    Rightbracket,
    Caret,
    Underscore,
    Backquote,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    Capslock,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    Printscreen,
    Scrolllock,
    Pause,
    Insert,
    Home,
    Pageup,
    Delete,
    End,
    Pagedown,
    Right,
    Left,
    Down,
    Up,

    Numlockclear,
    KP_Divide,
    KP_Multiply,
    KP_Minus,
    KP_Plus,
    KP_Enter,
    KP_1,
    KP_2,
    KP_3,
    KP_4,
    KP_5,
    KP_6,
    KP_7,
    KP_8,
    KP_9,
    KP_0,
    KP_Period,

    Lctrl,
    Lshift,
    Lalt,
    Rctrl,
    Rshift,
    Ralt,
};

Key FromSDLKey(SDL_Keycode const& code);

class EventKey : public Event {
public:
    EventKey(KeyAction action, Key key)
        : Event(GetStaticType())
        , m_action(action)
        , m_key(key) {}
    virtual ~EventKey() {}

    static constexpr int GetStaticType() { return EVENTTYPE_KEY; }

    KeyAction GetAction() const { return m_action; }
    Key GetKey() const { return m_key; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventKey>(m_action, m_key);
    }

private:
    KeyAction m_action;
    Key m_key;
};
