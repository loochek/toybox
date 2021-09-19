#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "Graphics.hpp"
#include "PhysicalCircle.hpp"
#include "Drawable.hpp"

/**
 * Drawable square with a circle physical model
 */
class Square : public PhysicalCircle, public Drawable
{
public:
    Square(const Vec2f &position = Vec2f(), float sideLength = 20.0f, const Vec2f &velocity = Vec2f(),
           const Color &color = Color(1.0f, 1.0f, 1.0f)) :
           PhysicalCircle(position, sideLength / 2, velocity), color(color) {};

protected:
    virtual void draw(Graphics &graphics) const;

    Color color;
};

#endif