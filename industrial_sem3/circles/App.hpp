#ifndef APP_HPP
#define APP_HPP

#include "Graphics.hpp"
#include "PhysicalWorld.hpp"

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
};

#endif