#include "DrawableCircle.hpp"

void DrawableCircle::draw(Graphics &graphics) const
{
    graphics.drawCircle(position, radius, color);
}