#ifndef PHYSICAL_CIRCLE_HPP
#define PHYSICAL_CIRCLE_HPP

#include "Vec2.hpp"

/**
 * Physical model of a circle
 */
class PhysicalCircle
{
public:
    PhysicalCircle(const Vec2f &position = Vec2f(), float radius = 1.0f, const Vec2f &velocity = Vec2f()) :
                   position(position), radius(radius), velocity(velocity) {};

public:
    Vec2f position;
    float radius;
    Vec2f velocity;
};

#endif