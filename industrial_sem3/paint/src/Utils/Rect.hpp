#ifndef RECT_HPP
#define RECT_HPP

#include "Vec2.hpp"

/**
 * Axis aligned rectangle
 */
template<typename T>
class Rect
{
public:
    Rect(const T &position = T(), const T &size = T()) : position(position), size(size) {};

    /**
     * Checks the point to be inside the rect
     * 
     * \param point Point
     */
    bool contains(const T &point) const
    {
        return point.x >= position.x && 
               point.y >= position.y &&
               point.x <= position.x + size.x &&
               point.y <= position.y + size.y;
    }

    /**
     * Checks another rect to be inside the rect
     * 
     * \param other Another rect
     */
    bool contains(const Rect<T> &other) const
    {
        return contains(other.position) &&
               contains(other.position + other.size);
    }

public:
    T position;
    T size;
};

typedef Rect<Vec2f> FloatRect;
typedef Rect<Vec2i> IntRect;

#endif