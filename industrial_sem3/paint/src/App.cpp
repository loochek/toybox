#include "App.hpp"
#include "GUIBase/WindowManager.hpp"
#include "GUIElements/PaintWindow.hpp"
#include "GUIElements/ColorPickerWindow.hpp"
#include "GUIElements/SizePickerWindow.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : mWindow(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)) //, texture1(Vec2i(400, 400)), texture2(Vec2i(300, 300)), texture3(Vec2i(100, 100)), texture4(Vec2i(100, 100))
{
    LGL::RenderTarget::loadFont();
    
    WindowManager *root = new WindowManager(IntRect(Vec2i(), Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), nullptr);
    PaintWindow *paintWindow = new PaintWindow(IntRect(Vec2i(200, 200), Vec2i(700, 400)), root);

    root->addChild(new ColorPickerWindow(Vec2i(1000, 500), paintWindow->getCanvas(), root));
    root->addChild(new SizePickerWindow (Vec2i(1000, 100), paintWindow->getCanvas(), root));
    root->addChild(paintWindow);
    mGuiManager = new GUIManager(mWindow, root);


    // // Nested render textures test
    
    // texture4.clear(LGL::Color::Cyan);
    // texture4.drawRect(FloatRect(Vec2i(25, 25), Vec2i(50, 50)));

    // texture3.clear(LGL::Color::Yellow);
    // texture3.drawRect(FloatRect(Vec2i(25, 25), Vec2i(50, 50)));

    // texture2.clear(LGL::Color::Magenta);
    // texture2.drawRenderTexture(texture4, Vec2f(50, 50));
    // texture2.drawRenderTexture(texture3, Vec2f(150, 150));

    // texture1.clear(LGL::Color::Red);
    // texture1.drawRenderTexture(texture2, Vec2f(50, 50));
}

App::~App()
{
    delete mGuiManager;
}

void App::run()
{
    while (!mWindow.shouldClose())
    {
        // Update
        
        float elapsedTime = mWindow.timerReset();

        mGuiManager->handleMouse();
        mGuiManager->update(elapsedTime);

        // Draw

        mWindow.clear();
        mGuiManager->draw();
        //mWindow.drawRenderTexture(texture1, Vec2f(300, 300));
        mWindow.display();
    }
}