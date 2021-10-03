#include "DrawableSquare.hpp"

void DrawableSquare::draw(Graphics &graphics, const Rect &viewport)
{
    Vec2f p1 = position + Vec2f(-radius, -radius);
    Vec2f p2 = position + Vec2f( radius, -radius);
    Vec2f p3 = position + Vec2f( radius,  radius);
    Vec2f p4 = position + Vec2f(-radius,  radius);
    
    if (viewport.contains(p1) && viewport.contains(p2) && viewport.contains(p3) && viewport.contains(p4))
        graphics.drawQuad(p1, p2, p3, p4, color);
}

void DrawableSquare::eventHandler(Event eventType, void *param1, void* param2)
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