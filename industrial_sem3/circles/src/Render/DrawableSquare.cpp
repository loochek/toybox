#include "DrawableSquare.hpp"

void DrawableSquare::draw(Graphics &graphics, const Rect &viewport)
{
    float boundCheckRadius = radius - VIEWPORT_THRESHOLD;

    Vec2f boundCheckP1 = viewport.position + position + Vec2f(-boundCheckRadius, -boundCheckRadius);
    Vec2f boundCheckP2 = viewport.position + position + Vec2f( boundCheckRadius, -boundCheckRadius);
    Vec2f boundCheckP3 = viewport.position + position + Vec2f( boundCheckRadius,  boundCheckRadius);
    Vec2f boundCheckP4 = viewport.position + position + Vec2f(-boundCheckRadius,  boundCheckRadius);
    
    Vec2f p1 = viewport.position + position + Vec2f(-radius, -radius);
    Vec2f p2 = viewport.position + position + Vec2f( radius, -radius);
    Vec2f p3 = viewport.position + position + Vec2f( radius,  radius);
    Vec2f p4 = viewport.position + position + Vec2f(-radius,  radius);
    
    if (viewport.contains(boundCheckP1) && viewport.contains(boundCheckP2) &&
        viewport.contains(boundCheckP3) && viewport.contains(boundCheckP4))
    {
        graphics.drawQuad(p1, p2, p3, p4, color);
    }
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