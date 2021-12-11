#include "App.hpp"
#include "TextureManager.hpp"
#include "EditorWidgets/PaintMainWindow.hpp"

#ifdef NDEBUG
const float FULLSCREEN = true;
#else
const float FULLSCREEN = false;
#endif

const int WINDOW_WIDTH  = 1920;
const int WINDOW_HEIGHT = 1080;

App::App() : mWindow(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT), "Pain", FULLSCREEN)
{
    LGL::RenderTarget::loadFont();
    TextureManager::getInstance()->load();
    
    // GUI initialization

    PaintMainWindow *root = new PaintMainWindow(IntRect(Vec2i(), Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), nullptr);
    mGuiManager = new GUIManager(mWindow, root);
}

App::~App()
{
    delete mGuiManager;
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