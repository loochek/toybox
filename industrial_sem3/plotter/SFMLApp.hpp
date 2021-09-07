#ifndef SFML_APP_HPP
#define SFML_APP_HPP

#include <SFML/Graphics.hpp>
#include "Plot.hpp"

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
    /// Specifies vectors to be drawn on the plot
    void handleVectors(float elapsedTime);

    Plot plot1;
    Plot plot2;

    sf::RenderWindow window;
    sf::Font font;

    double vectorsAngle;
};

#endif