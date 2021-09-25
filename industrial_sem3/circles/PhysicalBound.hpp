#ifndef PHYSICAL_BOUND_HPP
#define PHYSICAL_BOUND_HPP

#include "PhysicalObject.hpp"

enum class PhysicalBoundType
{
    Horizontal,
    Vertical
};

/**
 * Horizontal or vertical bound. Other objects bounce off it
 */
class PhysicalBound : public PhysicalObject
{
public:
    PhysicalBound(const Vec2f &position = Vec2f(), PhysicalBoundType boundType = PhysicalBoundType::Horizontal) :
                  PhysicalObject(PhysicalObjectType::Bound, position), boundType(boundType) {};

protected:
    /// Constructor used for inheritance
    PhysicalBound(PhysicalObjectType type,
                  const Vec2f &position = Vec2f(), PhysicalBoundType boundType = PhysicalBoundType::Horizontal) :
                  PhysicalObject(type, position), boundType(boundType) {};

public:
    PhysicalBoundType boundType;
};

#endif