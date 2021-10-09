#include "DrawableCircle.hpp"

void DrawableCircle::draw(Graphics &graphics, const Rect &viewport)
{
    float boundCheckRadius = radius - VIEWPORT_THRESHOLD;

    Vec2f p1 = viewport.position + position + Vec2f(0.0f, -boundCheckRadius);
    Vec2f p2 = viewport.position + position + Vec2f(0.0f,  boundCheckRadius);
    Vec2f p3 = viewport.position + position + Vec2f( boundCheckRadius, 0.0f);
    Vec2f p4 = viewport.position + position + Vec2f(-boundCheckRadius, 0.0f);
    
    if (viewport.contains(p1) && viewport.contains(p2) && viewport.contains(p3) && viewport.contains(p4))
        graphics.drawCircle(viewport.position + position, radius, color);
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