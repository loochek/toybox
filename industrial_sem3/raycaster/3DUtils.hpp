#ifndef _3DUTILS_HPP
#define _3DUTILS_HPP

/**
 * Stuff that can be useful for a 3D rendering
 */

#include <SFML/Graphics.hpp>
#include "Vec3.hpp"

const float EPS = 1e-6;
const float INF = 1e9;

typedef Vec3f Color;

struct Material
{
    Material(const Color &ambient  = Color(0.1f, 0.1f, 0.1f),
             const Color &diffuse  = Color(1.0f, 1.0f, 1.0f),
             const Color &specular = Color(1.0f, 1.0f, 1.0f),
             float specularFactor  = 10) :
                 ambient(ambient),
                 diffuse(diffuse),
                 specular(specular),
                 specularFactor(specularFactor) {};

    Color ambient;
    Color diffuse;
    Color specular;

    float specularFactor;
};

struct Sphere
{
    Sphere(const Vec3f &position    = Vec3f(),
           float radius             = 1.0f,
           const Material &material = Material()) :
               position(position), radius(radius), material(material) {};
    
    Vec3f position;
    float radius;

    Material material;
};

struct PointLight
{
    PointLight(const Vec3f &position = Vec3f(0.0f, 0.0f, 0.0f),
               const Color &ambient  = Vec3f(0.1f, 0.1f, 0.1f),
               const Color &diffuse  = Vec3f(1.0f, 1.0f, 1.0f),
               const Color &specular = Vec3f(1.0f, 1.0f, 1.0f)) :
                   position(position),
                   ambient(ambient),
                   diffuse(diffuse),
                   specular(specular) {};
    
    Vec3f position;
    
    Color ambient;
    Color diffuse;
    Color specular;
};


const Material MAT_COPPER(Color(0.19125f, 0.0735f, 0.0225),
                          Color(0.7038f, 0.27048f, 0.0828f),
                          Color(0.256777f, 0.137622f, 0.086014f),
                          10.0f);

const Material MAT_OBSIDIAN(Color(0.05375f, 0.05f, 0.06625f),
                            Color(0.18275f, 0.17f, 0.22525f),
                            Color(0.332741f, 0.328634f, 0.346435f),
                            30.0f);

const Material MAT_CHROME(Color(0.25f, 0.25f, 0.25f),
                          Color(0.4f, 0.4f, 0.4f),
                          Color(0.774597f, 0.774597f, 0.774597f),
                          60.0f);

const Material MAT_GOLD(Color(0.05f, 0.05f, 0.0f),
                         Color(0.5f, 0.5f, 0.4f),
                         Color(0.7f, 0.7f, 0.04f),
                         7.8125);

const Material MAT_LAMP(Color(1.0f, 1.0f, 1.0f),
                        Color(1.0f, 1.0f, 1.0f),
                        Color(1.0f, 1.0f, 1.0f),
                        0.0f);


/**
 * Checks intersection of a ray with a sphere
 * 
 * \param intersectionPointOut Where to write intersection point position if intersects
 * \return Intersection status
 */
bool raySphereIntersect(Vec3f rayOrigin, Vec3f rayDirection, const Sphere &sphere, Vec3f *intersectionPointOut);

/**
 * Constrains color components to 1.0f and converts color to SFML color
 */
sf::Color convertColor(const Color &color);

#endif