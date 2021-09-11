#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include <SFML/Graphics.hpp>
#include "3DUtils.hpp"

class Raycaster
{
public:
    Raycaster(sf::Vector2f position);

    void draw(sf::RenderTarget &target, float elapsedTime);

private:
    Color calculateColor(Vec3f cameraPosition, Vec3f surfacePosition, Vec3f normalVector);

    sf::Image   canvas;
    sf::Texture canvasTexture;
    sf::Sprite  canvasSprite;

    Vec3f screenOrigin;
    float screenWidth;
    float screenHeight;

    Sphere sphere;
    PointLight light;

    float timer;
};

#endif