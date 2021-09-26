#include "DrawableCircle.hpp"
#include "Entity.hpp"

void DrawableCircle::draw(Graphics &graphics)
{
    if (entity->physicalComponentPresent)
        position = entity->physicalComponent->position;

    graphics.drawCircle(position, radius, color);
}