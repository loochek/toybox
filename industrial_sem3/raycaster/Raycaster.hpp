#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include <SFML/Graphics.hpp>
#include "MathUtils.hpp"
#include "Vec3.hpp"

typedef MathUtils::Vec3f Color;

struct PointLight
{
    PointLight(MathUtils::Vec3f position, MathUtils::Vec3f color) : position(position), color(color) {};
    PointLight(MathUtils::Vec3f position) : position(position), color(MathUtils::Vec3f(1.0f, 1.0f, 1.0f)) {};
    PointLight() : color(MathUtils::Vec3f(1.0f, 1.0f, 1.0f)) {};

    MathUtils::Vec3f position;
    Color color;
};

class Raycaster
{
public:
    Raycaster(sf::Vector2f position);

    void draw(sf::RenderTarget &target, float elapsedTime);

private:
    Color calculateColor(MathUtils::Vec3f rayOrigin, MathUtils::Vec3f surfacePosition, MathUtils::Vec3f normal);

    sf::Image   canvas;
    sf::Texture canvasTexture;
    sf::Sprite  canvasSprite;

    MathUtils::Vec3f screenOrigin;
    float screenWidth;
    float screenHeight;

    MathUtils::Sphere sphere;
    Color sphereColor;

    PointLight light;
    Color      ambientLightColor;

    float timer;
};

#endif