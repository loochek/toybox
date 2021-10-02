#ifndef DRAWABLE_CIRCLE_HPP
#define DRAWABLE_CIRCLE_HPP

#include "../Graphics.hpp"
#include "Drawable.hpp"

/**
 * Drawable circle component
 */
class DrawableCircle: public Drawable
{
public:
    DrawableCircle(Entity *entity, const Vec2f &position = Vec2f(), float radius = 10.0f,
                   const Color &color = Color(1.0f, 1.0f, 1.0f)) :
                   Drawable(entity), position(position), radius(radius), color(color) {};

protected:
    virtual void draw(Graphics &graphics);

    /// Component event handler
    friend class Entity;
    virtual void eventHandler(Event eventType, void *param1, void* param2);

public:
    Vec2f position;
    float radius;

    Color color;
};

#endif