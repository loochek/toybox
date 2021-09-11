#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include <SFML/Graphics.hpp>
#include "3DUtils.hpp"

const int SPHERES_COUNT = 4;
const int LIGHTS_COUNT  = 2;

class Raycaster
{
public:
    Raycaster(int resolutionWidth = 640, int resolutionHeight = 360,
              sf::Vector2f canvasPosition = sf::Vector2f(0.0f, 0.0f));

    void setCanvasSize(sf::Vector2f size);

    void setCameraPosition(Vec3f position);
    void moveCamera(Vec3f offset);

    void update(float elapsedTime);
    void draw(sf::RenderTarget &target);

private:
    Color calculateColor(Vec3f fragmentPosition, Vec3f normal, const Material &material);

    sf::Image   canvas;
    sf::Texture canvasTexture;
    sf::Sprite  canvasSprite;

    sf::Vector2f canvasSize;

    int resolutionWidth;
    int resolutionHeight;

    Vec3f cameraPosition;

    /// Responsible to the camera FOV
    float screenDistance;
    float screenWidth;
    float screenHeight;

    Sphere     spheres[SPHERES_COUNT];
    PointLight lights[LIGHTS_COUNT];

    float lampAngle;
};

#endif