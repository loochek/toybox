#ifndef APP_HPP
#define APP_HPP

#include "SFMLGraphics.hpp"
#include "PhysicalWorld.hpp"
#include "Circle.hpp"

/**
 * App framework
 */
class App
{
public:
    App();
    ~App();

    /// Runs app until exit
    void run();

private:
    Graphics graphics;

    PhysicalWorld physicalWorld;

    Circle *circles;
};

#endif