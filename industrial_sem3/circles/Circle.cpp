#include "Circle.hpp"

void Circle::draw(Graphics &graphics) const
{
    graphics.drawCircle(position, radius, color);
}