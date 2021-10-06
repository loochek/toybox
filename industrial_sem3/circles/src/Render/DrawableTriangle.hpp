#ifndef DRAWABLE_TRIANGLE_HPP
#define DRAWABLE_TRIANGLE_HPP

#include "../Graphics.hpp"
#include "Drawable.hpp"

/**
 * Drawable square component
 */
class DrawableTriangle : public Drawable
{
public:
    DrawableTriangle(Entity *entity, const Vec2f &position = Vec2f(), float sideLength = 10.0f,
                     const Color &color = Color(1.0f, 1.0f, 1.0f)) :
                     Drawable(entity), position(position), sideLength(sideLength),
                     color(color) {};

protected:
    void draw(Graphics &graphics, const Rect &viewport) override;

    /// Component event handler
    friend class Entity;
    void eventHandler(Event eventType, void *param1, void* param2) override;

public:
    Vec2f position;
    float sideLength;
    Color color;
};

#endif