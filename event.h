#ifndef EVENT_H
#define EVENT_H

#include <cstdint>

struct MouseEvent
{
    enum class Button
    {
        Left,
        Right,
        Middle
    };
    int globalX;
    int globalY;
    int x;
    int y;
    Button b;
};

struct KeyEvent
{
    enum class Key
    {
        Left,
        Right,
        Up,
        Down,
        Unhandled,
        Count = Unhandled
    };
    Key key;

    inline bool operator <(const KeyEvent &other) const
    {
        return (key < other.key);
    }
};

#endif // EVENT_H
