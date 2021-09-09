#ifndef MATH_UTILS
#define MATH_UTILS

#include "Vec3.hpp"

namespace MathUtils
{
    struct Sphere
    {
        Sphere() : radius(0) {};
        Sphere(Vec3f position, float radius) : position(position), radius(radius) {};
        
        Vec3f position;
        float radius;
    };

    /**
     * Checks intersection of a ray with a sphere
     * 
     * \param intersectionPointOut Where to write intersection point position if intersects
     * \return Intersection status
     */
    bool raySphereIntersect(Vec3f rayOrigin, Vec3f rayDirection, Sphere sphere, Vec3f *intersectionPointOut);
}

#endif