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

    /**
     * Checks intersection with other circle
     * 
     * \param[in] other Other circle
     * \param[out] intersectionPoint Where to write intersection point if intersected
     * \return Intersection status
     */
    bool intersect(const PhysicalCircle &other, Vec2f &intersectionPoint) const;

public:
    Vec2f position;
    float radius;
    Vec2f velocity;
};

#endif