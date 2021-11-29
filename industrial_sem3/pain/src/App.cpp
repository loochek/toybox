#include "App.hpp"
#include "TextureManager.hpp"
#include "BaseGUI/WindowManager.hpp"
#include "BaseGUI/MenuBar.hpp"
#include "EditorLogic/PaintControllerMenuDelegate.hpp"
#include "EditorLogic/PaintController.hpp"

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
    bar->addButton("Open image", mDelegate, (int)MenuAction::OpenImageOpenWindow);
    bar->addButton("Pallete", mDelegate, (int)MenuAction::OpenPallete);
    bar->addButton("Size picker", mDelegate, (int)MenuAction::OpenSizePicker);
    bar->addButton("Tool picker", mDelegate, (int)MenuAction::OpenToolPicker);
    bar->addButton("Spline", mDelegate, (int)MenuAction::OpenSplineWindow);
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
    bool shouldClose = false;

    while (!shouldClose)
    {
        // Poll events

        LGL::Event event;
        while (mWindow.pollEvent(event))
        {
            switch (event.type)
            {
            case LGL::Event::Closed:
                shouldClose = true;
                break;

            case LGL::Event::KeyPressed:
            case LGL::Event::KeyReleased:
            case LGL::Event::MouseMoved:
            case LGL::Event::MouseButtonPressed:
            case LGL::Event::MouseButtonReleased:
            case LGL::Event::MouseWheelScrolled:
                mGuiManager->handleEvent(event);
                break;

            default:
                break;
            }
        }
        
        // Update
        
        float elapsedTime = mWindow.timerReset();
        //printf("FPS: %f\n", 1 / elapsedTime);

        mGuiManager->update(elapsedTime);

        // Draw

        mWindow.clear();
        mGuiManager->draw();
        mWindow.display();
    }
}