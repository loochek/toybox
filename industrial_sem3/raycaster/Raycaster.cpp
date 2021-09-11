#include <algorithm>
#include <math.h>
#include "Raycaster.hpp"

const int VIEWPORT_WIDTH  = 640;
const int VIEWPORT_HEIGHT = 640;

Raycaster::Raycaster(sf::Vector2f position) : screenOrigin(0.0f, -2.0f, 0.0f), screenWidth(2.0f),
                                              screenHeight(2.0f), sphere(Vec3f(0.0f, 0.0f, 0.0f), 1.0f)
{
    canvas.create(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    canvasSprite.setPosition(position);

    light.ambient  = Color(0.2f, 0.0f, 0.0f);
    light.diffuse  = Color(0.7f, 0.0f, 0.0f);
    light.specular = Color(1.0f, 0.0f, 0.0f);
}

void Raycaster::draw(sf::RenderTarget &target, float elapsedTime)
{
    timer += elapsedTime;
    light.position = Vec3f(4.0f * sin(timer), -2.0f + 4.0f * cos(timer), 2.0f);

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
            if (!raySphereIntersect(rayOrigin, rayDirection, sphere, &intersectionPoint))
            {
                canvas.setPixel(pixelX, pixelY, sf::Color::Black);
            }
            else
            {
                Color pixelColor = calculateColor(rayOrigin, intersectionPoint, intersectionPoint - sphere.position);
                sf::Color SFMLColor = sf::Color((int)(255.0f * pixelColor.x),
                                                (int)(255.0f * pixelColor.y),
                                                (int)(255.0f * pixelColor.z));
                canvas.setPixel(pixelX, pixelY, SFMLColor);
            }
        }
    }

    canvasTexture.loadFromImage(canvas);
    canvasSprite.setTexture(canvasTexture);
    target.draw(canvasSprite);
}

Color Raycaster::calculateColor(Vec3f rayOrigin, Vec3f surfacePosition, Vec3f normal)
{
    normal = normal.normalized();

    Color ambient = light.ambient * sphere.material.ambient;

    Vec3f lightVectorNorm = (light.position - surfacePosition).normalized();
    float diffuseIntensity = std::max(lightVectorNorm ^ normal, 0.0f);
    Color diffuse = light.diffuse * sphere.material.diffuse * diffuseIntensity;

    Vec3f viewVectorNorm  = (rayOrigin - surfacePosition).normalized();
    float specularIntensity = std::max(viewVectorNorm ^ (-lightVectorNorm).reflected(normal), 0.0f);
    Color specular = light.specular * pow(specularIntensity, sphere.material.specularFactor);

    Color resultColor = ambient + diffuse + specular;
    resultColor.x = std::min(resultColor.x, 1.0f);
    resultColor.y = std::min(resultColor.y, 1.0f);
    resultColor.z = std::min(resultColor.z, 1.0f);
    return resultColor;
}