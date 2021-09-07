#ifndef PLOT_HPP
#define PLOT_HPP

#include <SFML/Graphics.hpp>
#include "Vec2.hpp"

typedef double (*PlotFunction)(double x);

struct AppliedVector
{
    AppliedVector() : origin(Vec2(0, 0)), vector(Vec2(0, 0)) {};
    AppliedVector(Vec2 origin, Vec2 vector) : origin(origin), vector(vector) {};

    Vec2 origin;
    Vec2 vector;
};

class Plot
{
public:
    Plot();
    ~Plot();

    /**
     * Sets font for the text drawer
     * 
     * \param font Font
     */
    void setFont(sf::Font &font);

    /**
     * Sets viewport origin in plot coordinates
     * 
     * \param origin Viewport origin coordinates
     */
    void setViewportOrigin(Vec2 origin);

    /**
     * Sets viewport size for a plot
     * 
     * \param size Viewport size
     */
    void setViewportSize(sf::Vector2f size);

    /**
     * Sets viewport scale factor
     * 
     * \param scaleFactor Scale factor
     */
    void setScaleFactor(double scaleFactor);

    /**
     * Sets function to draw
     * 
     * \param function Function
     */
    void setFunction(PlotFunction function);

    /**
     * Moves viewport
     * 
     * \param offset Offset in plot coords
     */
    void moveViewport(Vec2 offset);

    /**
     * Scale viewport relative to offset
     * 
     * \param scale Scale factor
     */
    void scale(double scale);

    /**
     * Adds a vector to be drawn on the plot at the next draw call
     * 
     * \param vec Applied vector
     */
    void addVector(AppliedVector vec);

    /**
     * Draws the plot
     * 
     * \param canvas SFML render window
     * \param viewport_pos Viewport position
     */
    void draw(sf::RenderWindow &canvas, sf::Vector2f viewportPos);

private:

    void drawAxis   (sf::RenderWindow &canvas, sf::Vector2f viewportPos);
    void drawPlot   (sf::RenderWindow &canvas, sf::Vector2f viewportPos);
    void drawVectors(sf::RenderWindow &canvas, sf::Vector2f viewportPos);

    void drawLine(sf::RenderWindow &canvas,
                  sf::Vector2f p1, sf::Vector2f p2, float thickness, sf::Color color);

    void drawArrow(sf::RenderWindow &canvas,
                   sf::Vector2f p1, sf::Vector2f p2, float thickness, sf::Color color);

    PlotFunction function;

    /// Viewport size
    sf::Vector2f viewportSize;

    /// Viewport center coordinates
    Vec2 plotOrigin;

    /// SFML-to-plot coordinate multiplier
    double scaleFactor;

    sf::RectangleShape background;
    sf::CircleShape    dotDrawer;
    sf::ConvexShape    lineDrawer;
    sf::Text           textDrawer;

    /// Vectors to be drawn on the plot
    AppliedVector *vectors;
    int vecCount;

    // Some derivative values which are common-used

    /// Size of a plot fragment visible through viewport
    Vec2 plotSize;
    /// Viewport up corner coordinates
    Vec2 plotOffset;
};

#endif