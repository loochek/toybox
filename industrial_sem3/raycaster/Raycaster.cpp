#include <math.h>
#include "Raycaster.hpp"

const Color FILL_COLOR(0.0f, 0.2f, 0.2f);

Raycaster::Raycaster(int resolutionWidth, int resolutionHeight, sf::Vector2f canvasPosition) :
    resolutionWidth(resolutionWidth),
    resolutionHeight(resolutionHeight),
    cameraPosition(0.0f, -5.0f, 0.0f),
    screenWidth   (1.0f),
    screenHeight  (1.0f * resolutionHeight / resolutionWidth),
    screenDistance(1.0f)
{
    canvas.create(resolutionWidth, resolutionHeight);
    canvasSprite.setPosition(canvasPosition);

    // Copper sphere

    spheres[0].material = MAT_COPPER;
    spheres[0].position = Vec3f(2.0f, 0.0f, 0.0f);

    // Obsidian sphere

    spheres[1].material = MAT_OBSIDIAN;
    spheres[1].position = Vec3f(-2.0f, 0.0f, 0.0f);

    // Static lamp

    spheres[2].material = MAT_LAMP;
    spheres[2].position = Vec3f(0.0f, -2.5f, -1.0f);
    spheres[2].radius = 0.2f;

    lights[0].position  = spheres[2].position;

    // Dynamic lamp

    spheres[3].material = MAT_LAMP;
    spheres[3].radius = 0.2f;
}

void Raycaster::setCanvasSize(sf::Vector2f size)
{
    canvasSprite.setScale(sf::Vector2f(size.x / resolutionWidth, size.y / resolutionHeight));
    canvasSize = size;
}

void Raycaster::setCameraPosition(Vec3f position)
{
    cameraPosition = position;
}

void Raycaster::moveCamera(Vec3f offset)
{
    cameraPosition += offset;
}

void Raycaster::update(float elapsedTime)
{
    // Dynamic lamp

    lampAngle += elapsedTime;

    spheres[3].position = Vec3f(4.0f * sin(lampAngle), 4.0f * cos(lampAngle), 2.0f);
    lights[1].position = spheres[3].position;
}

void Raycaster::draw(sf::RenderTarget &target)
{
    // Position of the left up corner of the screen
    Vec3f screenPosition = cameraPosition + Vec3f(-screenWidth, screenDistance, screenHeight) / 2;

    float pixelWidth  = screenWidth  / resolutionWidth;
    float pixelHeight = screenHeight / resolutionHeight;

    for (int pixelY = 0; pixelY < resolutionHeight; pixelY++)
    {
        for (int pixelX = 0; pixelX < resolutionWidth; pixelX++)
        {
            Vec3f screenPixelPos = screenPosition + Vec3f(pixelWidth * pixelX, 0, -pixelHeight * pixelY);
            Vec3f rayDirection   = screenPixelPos - cameraPosition;

            Vec3f    fragmentPosition;
            Vec3f    fragmentNormal;
            float    fragmentDistance = INF;
            Material fragmentMaterial;

            for (int i = 0; i < SPHERES_COUNT; i++)
            {
                const Sphere &sphere = spheres[i];

                Vec3f intersectionPoint;
                if (raySphereIntersect(cameraPosition, rayDirection, sphere, &intersectionPoint))
                {
                    float dist = (intersectionPoint - cameraPosition).length();
                    if (dist < fragmentDistance)
                    {
                        fragmentDistance = dist;
                        fragmentPosition = intersectionPoint;
                        fragmentNormal   = (fragmentPosition - sphere.position).normalized();
                        fragmentMaterial = sphere.material;
                    }
                }
            }
                
            Color pixelColor;
            if (fragmentDistance == INF)
                pixelColor = FILL_COLOR;
            else
                pixelColor = calculateColor(fragmentPosition, fragmentNormal, fragmentMaterial);
            
            canvas.setPixel(pixelX, pixelY, convertColor(pixelColor));
        }
    }

    canvasTexture.loadFromImage(canvas);
    canvasSprite.setTexture(canvasTexture);
    target.draw(canvasSprite);
}

Color Raycaster::calculateColor(Vec3f fragmentPosition, Vec3f normal, const Material &material)
{
    normal = normal.normalized();

    Color resultColor;

    for (int i = 0; i < LIGHTS_COUNT; i++)
    {
        Color ambient = lights[i].ambient * material.ambient;

        Vec3f lightVectorNorm = (lights[i].position - fragmentPosition).normalized();
        float diffuseIntensity = std::max(lightVectorNorm ^ normal, 0.0f);
        Color diffuse = lights[i].diffuse * material.diffuse * diffuseIntensity;

        Vec3f viewVectorNorm = (cameraPosition - fragmentPosition).normalized();
        float specularIntensity = std::max(viewVectorNorm ^ (-lightVectorNorm).reflected(normal), 0.0f);
        specularIntensity = powf(specularIntensity, material.specularFactor);
        Color specular = lights[i].specular * material.specular * specularIntensity;

        resultColor += ambient;
        resultColor += diffuse;
        resultColor += specular;
    }

    return resultColor;
}