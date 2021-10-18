#ifndef APP_HPP
#define APP_HPP

#include "LGL/LGL.hpp"
#include "GUIBase/GUIManager.hpp"

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
    GUIManager        *mGuiManager;

    // LGL::RenderTexture texture1;
    // LGL::RenderTexture texture2;
    // LGL::RenderTexture texture3;
    // LGL::RenderTexture texture4;
};

#endif