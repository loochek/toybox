#ifndef DRAWABLE_SQUARE_HPP
#define DRAWABLE_SQUARE_HPP

#include "../Graphics.hpp"
#include "Drawable.hpp"

/**
 * Drawable square component
 */
class DrawableSquare : public Drawable
{
public:
    DrawableSquare(Entity *entity, const Vec2f &position = Vec2f(), float sideLength = 20.0f,
                   const Color &color = Color(1.0f, 1.0f, 1.0f)) :
                   Drawable(entity), position(position), radius(sideLength / 2),
                   color(color) {};

protected:
    virtual void draw(Graphics &graphics);

    /// Component event handler
    friend class Entity;
    virtual void eventHandler(Event eventType, void *param1, void* param2);

public:
    Vec2f position;
    /// Half side length
    float radius;
    Color color;
};

#endif