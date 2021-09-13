#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include <SFML/Graphics.hpp>
#include "3DUtils.hpp"

/**
 * Simple 3D visualizer
 */
class Raycaster
{
public:
    /**
     * \param target Where to display raycaster canvas
     * \param resolutionWidth Render resolution width
     * \param resolutionHeight Render resolution height
     * \param canvasPosition Raycaster canvas position relative to render targer
     */
    Raycaster(sf::RenderTarget &target, int resolutionWidth = 640, int resolutionHeight = 360,
              sf::Vector2f canvasPosition = sf::Vector2f(0.0f, 0.0f));
    ~Raycaster();

    /**
     * Sets raycaster canvas size
     * 
     * \param size New canvas size
     */
    void setCanvasSize(sf::Vector2f size);

    /**
     * Sets camera position
     * 
     * \param position New camera position
     */
    void setCameraPosition(Vec3f position);

    /**
     * Moves camera
     * 
     * \param offset Position delta
     */
    void moveCamera(Vec3f offset);

    /**
     * Registers sphere to be rendered on the next redraw
     * 
     * \param sphere Sphere to render
     */
    void sphereRendercall(const Sphere *sphere);

    /**
     * Registers point light to be rendered on the next redraw
     * 
     * \param light Light to render
     */
    void pointLightRendercall(const PointLight *light);

    /**
     * Redraws canvas with registered primitives and displays it on the associated render target
     */
    void draw();

private:
    Color calculateColor(Vec3f fragmentPosition, Vec3f normal, const Material &material);

    sf::RenderTarget &renderTarget;

    sf::Image   canvas;
    sf::Texture canvasTexture;
    sf::Sprite  canvasSprite;

    sf::Vector2f canvasSize;

    int resolutionWidth;
    int resolutionHeight;

    Vec3f cameraPosition;
    Vec3f cameraDirection;
    
    float screenWidth;
    float screenHeight;
    float screenDistance;

    const Sphere     **spheresRendercalls;
    const PointLight **lightsRendercalls;

    int lightsRendercallCount;
    int spheresRendercallCount;
};

#endif