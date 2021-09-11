#ifndef _3DUTILS_HPP
#define _3DUTILS_HPP

/**
 * Stuff that can be useful for a 3D rendering
 */

#include "Vec3.hpp"

typedef Vec3f Color;

struct Material
{
    Material() : ambient(0.1f, 0.1f, 0.1f), diffuse(1.0f, 1.0f, 1.0f), specularFactor(10) {};
    Material(const Color &ambient, const Color &diffuse, float specularFactor) :
                 ambient(ambient), diffuse(diffuse), specularFactor(specularFactor) {};

    Color ambient;
    Color diffuse;

    float specularFactor;
};

struct Sphere
{
    Sphere() : radius(0), position() {};
    Sphere(const Vec3f &position, float radius) : position(position), radius(radius) {};
    Sphere(const Vec3f &position, float radius, const Material &material) :
               position(position), radius(radius), material(material) {};
    
    Vec3f position;
    float radius;

    Material material;
};

struct PointLight
{
    PointLight() : position(0.0f, 0.0f, 0.0f), ambient(0.1f, 0.1f, 0.1f), diffuse(1.0f, 1.0f, 1.0f),
                   specular(1.0f, 1.0f, 1.0f) {};
    PointLight(const Vec3f &position) : position(position), ambient(0.1f, 0.1f, 0.1f),
                   diffuse(1.0f, 1.0f, 1.0f), specular(1.0f, 1.0f, 1.0f) {};

    PointLight(const Vec3f &position, const Color &ambient, const Color &diffuse, const Color &specular) :
                   position(position), ambient(0.1f, 0.1f, 0.1f), diffuse(1.0f, 1.0f, 1.0f),
                   specular(1.0f, 1.0f, 1.0f) {};
    
    Vec3f position;
    
    Color ambient;
    Color diffuse;
    Color specular;
};

/**
 * Checks intersection of a ray with a sphere
 * 
 * \param intersectionPointOut Where to write intersection point position if intersects
 * \return Intersection status
 */
bool raySphereIntersect(Vec3f rayOrigin, Vec3f rayDirection, Sphere sphere, Vec3f *intersectionPointOut);

#endif