#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include <SFML/Graphics.hpp>
#include "MathUtils.hpp"
#include "Vec3.hpp"

class Raycaster
{
public:
    Raycaster(sf::Vector2f position);
    Raycaster();

    void Draw(sf::RenderTarget &target);

private:
    sf::Image   canvas;
    sf::Texture canvasTexture;
    sf::Sprite  canvasSprite;

    MathUtils::Vec3f screenOrigin;
    float screenWidth;
    float screenHeight;

    MathUtils::Sphere sphere;
};

#endif