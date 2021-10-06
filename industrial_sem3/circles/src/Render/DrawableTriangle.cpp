#include "DrawableTriangle.hpp"

const float sqrt3 = sqrt(3);

void DrawableTriangle::draw(Graphics &graphics, const Rect &viewport)
{
    Vec2f p1 = viewport.position + position + Vec2f(0.0f, -sideLength / sqrt3);
    Vec2f p2 = viewport.position + position + Vec2f(sideLength / 2, sideLength / (2 * sqrt3));
    Vec2f p3 = viewport.position + position + Vec2f(-sideLength / 2, sideLength / (2 * sqrt3));
    
    if (viewport.contains(p1) && viewport.contains(p2) && viewport.contains(p3))
        graphics.drawTriangle(p1, p2, p3, color);
}

void DrawableTriangle::eventHandler(Event eventType, void *param1, void* param2)
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