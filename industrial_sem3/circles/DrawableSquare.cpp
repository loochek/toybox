#include "DrawableSquare.hpp"
#include "Entity.hpp"

void DrawableSquare::draw(Graphics &graphics)
{
    if (entity->physicalComponentPresent)
        position = entity->physicalComponent->position;

    Vec2f p1 = position + Vec2f(-radius, -radius);
    Vec2f p2 = position + Vec2f( radius, -radius);
    Vec2f p3 = position + Vec2f( radius,  radius);
    Vec2f p4 = position + Vec2f(-radius,  radius);
    
    graphics.drawQuad(p1, p2, p3, p4, color);
}