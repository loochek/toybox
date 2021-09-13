#include <math.h>
#include <cassert>
#include "Raycaster.hpp"

const int MAX_SPHERES_RENDERCALLS = 10;
const int MAX_LIGHTS_RENDERCALLS  = 10;

const Color FILL_COLOR(0.0f, 0.2f, 0.2f);

Raycaster::Raycaster(sf::RenderTarget &target, int resolutionWidth, int resolutionHeight,
                     sf::Vector2f canvasPosition) :
    renderTarget          (target),
    resolutionWidth       (resolutionWidth),
    resolutionHeight      (resolutionHeight),
    screenWidth           (1.0f),
    screenHeight          (1.0f * resolutionHeight / resolutionWidth),
    screenDistance        (2.0f),
    cameraPosition        (0.0f, -15.0f, 0.0f),
    cameraDirection       (0.0f, 1.0f, 0.0f),
    spheresRendercallCount(0),
    lightsRendercallCount (0)
{
    spheresRendercalls = new const Sphere*    [MAX_SPHERES_RENDERCALLS];
    lightsRendercalls  = new const PointLight*[MAX_LIGHTS_RENDERCALLS];

    canvas.create(resolutionWidth, resolutionHeight);
    canvasSprite.setPosition(canvasPosition);
}

Raycaster::~Raycaster()
{
    delete[] spheresRendercalls;
    delete[] lightsRendercalls;
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

void Raycaster::sphereRendercall(const Sphere *sphere)
{
    assert(sphere != nullptr);

    if (spheresRendercallCount >= MAX_SPHERES_RENDERCALLS)
        return;

    spheresRendercalls[spheresRendercallCount++] = sphere;
}

void Raycaster::pointLightRendercall(const PointLight *light)
{
    assert(light != nullptr);

    if (lightsRendercallCount >= MAX_LIGHTS_RENDERCALLS)
        return;

    lightsRendercalls[lightsRendercallCount++] = light;
}

void Raycaster::draw()
{
    Vec3f screenVectorX = (Vec3f(0.0f, 0.0f, -1.0f) & cameraDirection).normalized();
    Vec3f screenVectorY = (cameraDirection & screenVectorX).normalized();

    // Position of the left up corner of the screen
    Vec3f screenPosition = cameraPosition
                           + cameraDirection * screenDistance
                           - screenVectorX * (screenWidth / 2)
                           - screenVectorY * (screenHeight / 2);

    screenVectorX *= screenWidth  / resolutionWidth;
    screenVectorY *= screenHeight / resolutionHeight;

    for (int pixelY = 0; pixelY < resolutionHeight; pixelY++)
    {
        for (int pixelX = 0; pixelX < resolutionWidth; pixelX++)
        {
            Vec3f screenPixelPos = screenPosition + screenVectorX * pixelX + screenVectorY * pixelY;
            Vec3f rayDirection   = screenPixelPos - cameraPosition;

            Vec3f    fragmentPosition;
            Vec3f    fragmentNormal;
            float    fragmentDistance = INF;
            Material fragmentMaterial;

            for (int i = 0; i < spheresRendercallCount; i++)
            {
                const Sphere *sphere = spheresRendercalls[i];

                Vec3f intersectionPoint;
                if (raySphereIntersect(cameraPosition, rayDirection, *sphere, &intersectionPoint))
                {
                    float dist = (intersectionPoint - cameraPosition).length();
                    if (dist < fragmentDistance)
                    {
                        fragmentDistance = dist;
                        fragmentPosition = intersectionPoint;
                        fragmentNormal   = (fragmentPosition - sphere->position).normalized();
                        fragmentMaterial = sphere->material;

                        // prekol
                        //srand(fragmentPosition.x * 31 + fragmentPosition.y * 31 * 31 + fragmentPosition.z * 31 * 31 * 31);
                        //fragmentNormal += Vec3f(rand() * 0.00000000013, rand() * 0.00000000013, rand() * 0.00000000013);
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
    renderTarget.draw(canvasSprite);

    spheresRendercallCount = 0;
    lightsRendercallCount  = 0;
}

Color Raycaster::calculateColor(Vec3f fragmentPosition, Vec3f normal, const Material &material)
{
    normal = normal.normalized();

    Color resultColor;

    for (int i = 0; i < lightsRendercallCount; i++)
    {
        Color ambient = lightsRendercalls[i]->ambient * material.ambient;

        Vec3f lightVectorNorm = (lightsRendercalls[i]->position - fragmentPosition).normalized();
        float diffuseIntensity = std::max(lightVectorNorm ^ normal, 0.0f);
        Color diffuse = lightsRendercalls[i]->diffuse * material.diffuse * diffuseIntensity;

        Vec3f viewVectorNorm = (cameraPosition - fragmentPosition).normalized();
        float specularIntensity = std::max(viewVectorNorm ^ (-lightVectorNorm).reflected(normal), 0.0f);
        specularIntensity = powf(specularIntensity, material.specularFactor);
        Color specular = lightsRendercalls[i]->specular * material.specular * specularIntensity;

        resultColor += ambient;
        resultColor += diffuse;
        resultColor += specular;
    }

    return resultColor;
}