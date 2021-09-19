#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "Graphics.hpp"
#include "PhysicalCircle.hpp"
#include "Drawable.hpp"

/**
 * Drawable circle with a physical model
 */
class Circle : public PhysicalCircle, public Drawable
{
public:
    Circle(const Vec2f &position = Vec2f(), float radius = 10.0f, const Vec2f &velocity = Vec2f(),
           const Color &color = Color(1.0f, 1.0f, 1.0f)) :
           PhysicalCircle(position, radius, velocity), color(color) {};

protected:
    virtual void draw(Graphics &graphics) const;

    Color color;
};

#endif