#include "Raycaster.hpp"

using MathUtils::Vec3f;

const int VIEWPORT_WIDTH  = 500;
const int VIEWPORT_HEIGHT = 500;

Raycaster::Raycaster(sf::Vector2f position)
{
    canvas.create(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    canvasSprite.setPosition(position);

    screenOrigin = Vec3f(0.0f, -2.0f, 0.0f);
    screenWidth  = 2.0f;
    screenHeight = 2.0f;

    sphere.position = Vec3f(0.0f, 0.0f, 0.0f);
    sphere.radius   = 1.0f;
}

void Raycaster::Draw(sf::RenderTarget &target)
{
    Vec3f rayDirection = Vec3f(0.0f, 1.0f, 0.0f);

    // Position of the left up corner of the screen
    Vec3f screenPosition = screenOrigin + Vec3f(-screenWidth, 0, screenHeight) / 2;

    float pixelWidth  = screenWidth  / VIEWPORT_WIDTH;
    float pixelHeight = screenHeight / VIEWPORT_HEIGHT;

    for (int pixelY = 0; pixelY < VIEWPORT_HEIGHT; pixelY++)
    {
        for (int pixelX = 0; pixelX < VIEWPORT_WIDTH; pixelX++)
        {
            Vec3f rayOrigin = screenPosition + Vec3f(pixelWidth * pixelX, 0, -pixelHeight * pixelY);

            Vec3f intersectionPoint;
            if (!MathUtils::raySphereIntersect(rayOrigin, rayDirection, sphere, &intersectionPoint))
            {
                canvas.setPixel(pixelX, pixelY, sf::Color::Black);
            }
            else
            {
                canvas.setPixel(pixelX, pixelY, sf::Color::Red);
            }
        }
    }

    canvasTexture.loadFromImage(canvas);
    canvasSprite.setTexture(canvasTexture);
    target.draw(canvasSprite);
}