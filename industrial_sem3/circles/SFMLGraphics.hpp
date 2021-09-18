#ifndef SFML_GRAPHICS_H
#define SFML_GRAPHICS_H

#include <SFML/Graphics.hpp>
#include "Vec2.hpp"
#include "Vec3.hpp"

typedef Vec3f Color;

/**
 * SFML implementation of graphics abstraction
 */
class Graphics
{
public:
    /**
     * Creates an abstract drawing window
     * 
     * \param resolution Window resolution
     */
    Graphics(const Vec2i &resolution);

    ~Graphics();

    /**
     * Draws a circle
     * 
     * \param position Circle position
     * \param radius Circle radius
     * \param color Circle color
     */
    void drawCircle(const Vec2f &position, float radius = 1.0f, const Color &color = Color(1.0f, 1.0f, 1.0f));

    /**
     * Clears the screen
     * 
     * \param clearColor Clear color
     */
    void clear(const Color &clearColor = Color());

    /**
     * Displays drawn primitives on the screen
     */
    void display();

    /**
     * Checks for a window pending close from a graphics library
     */
    bool shouldClose();

    /**
     * Resets timer
     * 
     * \return Time in seconds from the last reset
     */
    float timerReset();

private:
    sf::Vector2f toSFMLVector(const Vec2f &vector);
    sf::Color    toSFMLColor (const Color &color);
    
    sf::RenderWindow window;
    sf::CircleShape circleDrawer;

    sf::Clock clock;
};

#endif