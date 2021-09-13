#include <algorithm>
#include <cassert>
#include "3DUtils.hpp"

bool raySphereIntersect(Vec3f rayOrigin, Vec3f rayDirection, const Sphere &sphere, Vec3f *intersectionPointOut)
{
    assert(intersectionPointOut != nullptr);

    /*
        Sphere equation - |x - sphere.position|^2 = r^2
        Ray equation    - r = rayOrigin + rayDirection*t
        Substitution    - |rayOrigin + rayDirection*t - sphere.position|^2 = r^2
        Got square equation with coeffs:
            A = |rayDirection|^2
            B = 2*|(rayOrigin - sphere.position)*rayDirection|
            C = |rayOrigin - sphere.position|^2 - sphere.radius^2
        Solutions - t values of intersection points
    */
    
    Vec3f vecS = rayOrigin - sphere.position;

    float coeffA = rayDirection ^ rayDirection;
    float coeffB = 2 * (vecS ^ rayDirection);
    float coeffC = (vecS ^ vecS) - sphere.radius * sphere.radius;

    float discr = coeffB * coeffB - 4 * coeffA * coeffC;

    float t = -1.0f;

    if (fabsf(discr) < -EPS)
    {
        // No intersection points
        return false;
    }
    else if (fabsf(discr) < EPS)
    {
        // One solution
        float t1 = -coeffB / (2 * coeffA);

        // Only positive solutions are suitable
        if (t1 > EPS)
            t = t1;
    }
    else
    {
        // Two solutions
        float t1 = (-coeffB - sqrtf(discr)) / (2 * coeffA);
        float t2 = (-coeffB + sqrtf(discr)) / (2 * coeffA);

        // Only positive solutions are suitable, smaller one is preferable
        if (t1 > EPS)
            t = t1;
        else if (t2 > EPS)
            t = t2;
    }

    if (t < 0)
    {
        // No suitable solutions found
        return false;
    }

    *intersectionPointOut = rayOrigin + rayDirection * t;
    return true;
}

sf::Color convertColor(const Color &color)
{
    return sf::Color(std::min(color.x, 1.0f) * 255,
                     std::min(color.y, 1.0f) * 255,
                     std::min(color.z, 1.0f) * 255);
}