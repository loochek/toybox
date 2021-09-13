#ifndef SFML_APP_HPP
#define SFML_APP_HPP

#include <SFML/Graphics.hpp>
#include "Raycaster.hpp"
#include "ButtonOverlay.hpp"

const int SPHERES_COUNT = 4;
const int LIGHTS_COUNT  = 2;

/**
 * SFML app framework
 */
class SFMLApp
{
public:
    SFMLApp();

    /// Runs SFML app until exit
    void run();

private:
    sf::RenderWindow window;
    sf::Font         font;

    ///////////////////////////////

    Raycaster     raycaster;

    Sphere     spheres[SPHERES_COUNT];
    PointLight lights [LIGHTS_COUNT];
    
    int spheresMaterial[SPHERES_COUNT];

    float lampAngle;

    ///////////////////////////////

    ButtonOverlay buttonOverlay;

    ButtonHandle forwardButton;
    ButtonHandle backButton;
    ButtonHandle leftButton;
    ButtonHandle rightButton;

    ButtonHandle upButton;
    ButtonHandle downButton;

    ButtonHandle spheresMatPickers[2];
};

#endif