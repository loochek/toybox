#include "App.hpp"
#include "GUI/MainWidget.hpp"
#include "GUI/Window.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : mWindow(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)) //, texture1(Vec2i(400, 400)), texture2(Vec2i(300, 300)), texture3(Vec2i(200, 200)), texture4(Vec2i(100, 100))
{
    MainWidget *root = new MainWidget(IntRect(Vec2i(), Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), nullptr);
    root->addChild(new Window(IntRect(Vec2i(200, 200), Vec2i(700, 400)), root));
    mGuiManager = new GUIManager(mWindow, root);

    // Nested render textures test
    //
    // texture4.clear(LGL::Color::Cyan);
    // texture4.drawRect(FloatRect(Vec2i(25, 25), Vec2i(50, 50)));

    // texture3.clear(LGL::Color::Yellow);
    // texture3.drawRenderTexture(texture4, Vec2f(50, 50));

    // texture2.clear(LGL::Color::Magenta);
    // texture2.drawRenderTexture(texture3, Vec2f(50, 50));

    // texture1.clear(LGL::Color::Red);
    // texture1.drawRenderTexture(texture2, Vec2f(50, 50));
}

App::~App()
{
    //delete mGuiManager;
}

void App::run()
{
    while (!mWindow.shouldClose())
    {
        // Update
        
        float elapsedTime = mWindow.timerReset();

        mGuiManager->update(elapsedTime);

        // Draw

        mWindow.clear();
        //mWindow.drawRenderTexture(texture1, Vec2f(300, 300));
        mGuiManager->draw();
        mWindow.display();
    }
}