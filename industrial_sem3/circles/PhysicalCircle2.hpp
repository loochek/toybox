#ifndef PHYSICAL_CIRCLE_2_HPP
#define PHYSICAL_CIRCLE_2_HPP

#include "PhysicalCircle.hpp"

/**
 * Another component of a physical circle, but with different collision behavior
 */
class PhysicalCircle2 : public PhysicalCircle
{
public:
    PhysicalCircle2(Entity *entity, const Vec2f &position = Vec2f(), float radius = 1.0f,
                    const Vec2f &velocity = Vec2f()) :
                    PhysicalCircle(entity, PhysicalObjectType::Circle2, position, radius, velocity) {};

protected:
    /// Constructor used for inheritance
    PhysicalCircle2(Entity *entity, PhysicalObjectType type,
                    const Vec2f &position = Vec2f(), float radius = 1.0f,
                    const Vec2f &velocity = Vec2f()) :
                    PhysicalCircle(entity, type, position, radius, velocity) {};
};

#endif