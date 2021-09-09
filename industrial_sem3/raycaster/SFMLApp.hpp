#ifndef SFML_APP_HPP
#define SFML_APP_HPP

#include <SFML/Graphics.hpp>
#include "Raycaster.hpp"

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

    Raycaster raycaster;
};

#endif