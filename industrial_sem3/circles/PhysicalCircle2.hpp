#ifndef PHYSICAL_CIRCLE_2_HPP
#define PHYSICAL_CIRCLE_2_HPP

#include "PhysicalCircle.hpp"

/**
 * Another model of a circle, but with different collision behavior
 */
class PhysicalCircle2 : public PhysicalCircle
{
public:
    PhysicalCircle2(const Vec2f &position = Vec2f(), float radius = 1.0f, float mass = 1.0f,
                    const Vec2f &velocity = Vec2f()) :
                    PhysicalCircle(PhysicalObjectType::Circle2, position, radius, mass, velocity) {};
};

#endif