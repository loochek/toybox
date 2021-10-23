#include "App.hpp"
#include "TextureManager.hpp"
#include "GUIBase/WindowManager.hpp"
#include "GUIElements/PaintWindow.hpp"
#include "GUIElements/ColorPickerWindow.hpp"
#include "GUIElements/SizePickerWindow.hpp"
#include "GUIBase/MenuBar.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : mWindow(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT))
{
    LGL::RenderTarget::loadFont();
    TextureManager::getInstance()->load();
    
    WindowManager *root = new WindowManager(IntRect(Vec2i(), Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), nullptr);
    PaintWindow *paintWindow = new PaintWindow(IntRect(Vec2i(200, 200), Vec2i(700, 400)), root);

    MenuBar *bar = new MenuBar(root);
    bar->addButton("New canvas", nullptr);
    bar->addButton("Pallete", nullptr);
    bar->addButton("Size picker", nullptr);
    root->addChild(bar);
    root->addChild(new ColorPickerWindow(Vec2i(1000, 500), paintWindow->getCanvas(), root));
    root->addChild(new SizePickerWindow (Vec2i(1000, 100), paintWindow->getCanvas(), root));
    root->addChild(paintWindow);
    mGuiManager = new GUIManager(mWindow, root);
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
        //printf("FPS: %f\n", 1 / elapsedTime);

        mGuiManager->handleMouse();
        mGuiManager->update(elapsedTime);

        // Draw

        mWindow.clear();
        mGuiManager->draw();
        mWindow.display();
    }
}