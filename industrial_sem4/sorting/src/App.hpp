#ifndef APP_HPP
#define APP_HPP

#include "LGL/LGL.hpp"
#include "BaseGUI/GUIManager.hpp"
#include "Utils/Chart.hpp"

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

    // Force redraw
    void redraw();

private:
    LGL::RenderWindow mWindow;
    GUIManager       *mGuiManager;
};

#endif