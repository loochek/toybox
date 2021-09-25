#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "Graphics.hpp"
#include "PhysicalCircle2.hpp"
#include "Drawable.hpp"

/**
 * Drawable square with a circle physical model
 */
class Square : public PhysicalCircle2, public Drawable
{
public:
    Square(const Vec2f &position = Vec2f(), float sideLength = 20.0f, float mass = 1.0f,
           const Vec2f &velocity = Vec2f(), const Color &color = Color(1.0f, 1.0f, 1.0f)) :
           PhysicalCircle2(position, sideLength / 2, mass, velocity), color(color) {};

protected:
    virtual void draw(Graphics &graphics) const;

    Color color;
};

#endif