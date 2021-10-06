#ifndef INERT_PHYSICAL_CIRCLE_HPP
#define INERT_PHYSICAL_CIRCLE_HPP

#include "PhysicalCircle.hpp"

/**
 * Another component of a physical circle, but with different collision behavior
 */
class InertPhysicalCircle : public PhysicalCircle
{
public:
    InertPhysicalCircle(Entity *entity, const Vec2f &position = Vec2f(), float radius = 1.0f,
                        const Vec2f &velocity = Vec2f()) :
                        PhysicalCircle(entity, PhysicalObjectType::InertCircle, position, radius, velocity) {};

protected:
    /// Constructor used for inheritance
    InertPhysicalCircle(Entity *entity, PhysicalObjectType type,
                       const Vec2f &position = Vec2f(), float radius = 1.0f,
                       const Vec2f &velocity = Vec2f()) :
                       PhysicalCircle(entity, type, position, radius, velocity) {};
};

#endif