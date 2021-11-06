#include "App.hpp"
#include "TextureManager.hpp"
#include "GUIBase/WindowManager.hpp"
#include "GUIBase/MenuBar.hpp"
#include "GUILogic/PaintControllerMenuDelegate.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : mWindow(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT), "Pain")
{
    LGL::RenderTarget::loadFont();
    TextureManager::getInstance()->load();
    
    // GUI initialization

    WindowManager *root = new WindowManager(IntRect(Vec2i(), Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), nullptr);
    
    mPaintController = new PaintController(root);

    mDelegate = new PaintControllerMenuDelegate(mPaintController);

    MenuBar *bar = new MenuBar(root);
    bar->addButton("New canvas", mDelegate, (int)MenuAction::NewCanvas);
    bar->addButton("Pallete", mDelegate, (int)MenuAction::OpenPallete);
    bar->addButton("Size picker", mDelegate, (int)MenuAction::OpenSizePicker);
    root->addChild(bar);

    mGuiManager = new GUIManager(mWindow, root);
}

App::~App()
{
    delete mGuiManager;
    delete mDelegate;
    delete mPaintController;
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