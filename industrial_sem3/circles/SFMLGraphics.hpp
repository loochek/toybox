#ifndef SFML_GRAPHICS_H
#define SFML_GRAPHICS_H

#include <SFML/Graphics.hpp>
#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Drawable.hpp"

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
     * Draws a drawable object
     * 
     * \param drawable Drawable object
     */
    void draw(const Drawable &drawable);

    /**
     * Draws a quad by points
     * 
     * \param p1 Point 1
     * \param p2 Point 2
     * \param p3 Point 3
     * \param p4 Point 4
     * \param color Quad color
     */
    void drawQuad(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4,
                  const Color &color = Color(1.0f, 1.0f, 1.0f));

    /**
     * Draws a circle
     * 
     * \param position Circle position
     * \param radius Circle radius
     * \param color Circle color
     */
    void drawCircle(const Vec2f &position, float radius = 1.0f, const Color &color = Color(1.0f, 1.0f, 1.0f));

    /**
     * Draws a line segment specified by two points
     * 
     * \param p1 First point
     * \param p2 Second point
     * \param thickness Thickness
     * \param color Color
     */
    void drawLine(const Vec2f &p1, const Vec2f &p2, float thickness = 1.0f,
                  const Color &color = Color(1.0f, 1.0f, 1.0f));

    /**
     * Draws an arrow specified by two points
     * 
     * \param start Start point
     * \param end End point
     * \param thickness Thickness
     * \param color Color
     */
    void drawArrow(const Vec2f &start, const Vec2f &end, float thickness = 1.0f,
                   const Color &color = Color(1.0f, 1.0f, 1.0f));

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
    static sf::Vector2f toSFMLVector(const Vec2f &vector);
    static sf::Color    toSFMLColor (const Color &color);
    
    sf::RenderWindow window;
    sf::CircleShape  circleDrawer;
    sf::ConvexShape  polygonDrawer;

    sf::Clock clock;
};

#endif