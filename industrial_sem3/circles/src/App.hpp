#ifndef APP_HPP
#define APP_HPP

#include "Graphics.hpp"
#include "GUI/GUIManager.hpp"

class CirclesWidget;
class Chart;

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
    GUIManager guiManager;

    CirclesWidget *circlesWidget;
    Chart *chart;
};

#endif