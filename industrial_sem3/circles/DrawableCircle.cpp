#include "DrawableCircle.hpp"
#include "Entity.hpp"

void DrawableCircle::draw(Graphics &graphics)
{
    graphics.drawCircle(position, radius, color);
}

void DrawableCircle::eventHandler(Event eventType, void *param1, void* param2)
{
    switch (eventType)
    {
    case Event::PhysicalPositionChanged:
    {
        const Vec2f *newPosition = (const Vec2f*)param1;
        position = *newPosition;
        break;
    }

    default:
        break;
    }
}