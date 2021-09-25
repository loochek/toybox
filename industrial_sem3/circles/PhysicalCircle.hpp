#ifndef PHYSICAL_CIRCLE_HPP
#define PHYSICAL_CIRCLE_HPP

#include "PhysicalObject.hpp"

/**
 * Component of a physical circle
 */
class PhysicalCircle : public PhysicalObject
{
public:
    PhysicalCircle(Entity *entity, const Vec2f &position = Vec2f(), float radius = 1.0f, float mass = 1.0f,
                   const Vec2f &velocity = Vec2f()) :
                   PhysicalObject(entity, PhysicalObjectType::Circle, position, mass, velocity),
                   radius(radius) {};

protected:
    /// Constructor used for inheritance
    PhysicalCircle(Entity *entity, PhysicalObjectType type,
                   const Vec2f &position = Vec2f(), float radius = 1.0f, float mass = 1.0f,
                   const Vec2f &velocity = Vec2f()) :
                   PhysicalObject(entity, type, position, mass, velocity), radius(radius) {};

public:
    float radius;
};

#endif