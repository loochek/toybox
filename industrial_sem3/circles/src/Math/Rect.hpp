#ifndef RECT_HPP
#define RECT_HPP

#include "Vec2.hpp"

/**
 * Axis aligned rectangle
 */
class Rect
{
public:
    Rect(const Vec2f &position = Vec2f(), const Vec2f &size = Vec2f());

    /**
     * Checks the point to be inside the rect
     * 
     * \param point Point
     */
    bool contains(const Vec2f &point) const;

public:
    Vec2f position;
    Vec2f size;
};

#endif