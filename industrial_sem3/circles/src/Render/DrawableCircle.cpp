#include "DrawableCircle.hpp"

void DrawableCircle::draw(Graphics &graphics, const Rect &viewport)
{
    Vec2f p1 = position + Vec2f(0.0f, -radius);
    Vec2f p2 = position + Vec2f(0.0f,  radius);
    Vec2f p3 = position + Vec2f( radius, 0.0f);
    Vec2f p4 = position + Vec2f(-radius, 0.0f);
    
    if (viewport.contains(p1) && viewport.contains(p2) && viewport.contains(p3) && viewport.contains(p4))
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