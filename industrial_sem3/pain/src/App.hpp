#ifndef APP_HPP
#define APP_HPP

#include "LGL/LGL.hpp"
#include "BaseGUI/GUIManager.hpp"

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
    LGL::RenderWindow mWindow;
    GUIManager       *mGuiManager;
};

#endif