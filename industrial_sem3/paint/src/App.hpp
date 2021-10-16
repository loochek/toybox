#ifndef APP_HPP
#define APP_HPP

#include "LGL/LGL.hpp"
//#include "GUI/GUIManager.hpp"

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
    LGL::RenderWindow window;
    LGL::RenderTexture rt;
    //GUIManager guiManager;
};

#endif