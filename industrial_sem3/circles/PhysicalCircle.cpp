#include "PhysicalCircle.hpp"

bool PhysicalCircle::intersect(const PhysicalCircle &other, Vec2f &intersectionPoint) const
{
    Vec2f lengthVector = other.position - position;
    float intersectLength = radius + other.radius;

    if ((lengthVector ^ lengthVector) > intersectLength * intersectLength)
        return false;

    intersectionPoint = position + lengthVector.normalized() * radius;
    return true;
}

void PhysicalCircle::update(float elapsedTime)
{
    position += velocity * elapsedTime;
}