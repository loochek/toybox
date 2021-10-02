#include "Rect.hpp"

Rect::Rect(const Vec2f &position, const Vec2f &size) : position(position), size(size)
{
}

bool Rect::contains(const Vec2f &point)
{
    return point.x >= position.x && 
           point.y >= position.y &&
           point.x <= position.x + size.x &&
           point.y <= position.y + size.y;
}