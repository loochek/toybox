#include "Circle.hpp"

void Circle::draw(Graphics &graphics)
{
    graphics.drawCircle(position, radius, color);
}