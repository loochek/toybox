#include "App.hpp"
#include "TextureManager.hpp"
#include "BaseGUI/WindowManager.hpp"
#include "SortingWidgets/AnalysisWindow.hpp"
#include "SortingWidgets/VisualSortWindow.hpp"

const Vec2i ANALYSIS_WINDOW_POS(200, 200);
const Vec2i VISUAL_SORT_WINDOW_POS(300, 300);

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : mWindow(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT), "amogus", false)
{
    LGL::RenderTarget::loadFont();
    TextureManager::getInstance()->load();
    
    // GUI initialization

    WindowManager *root = new WindowManager(IntRect(Vec2i(), Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), nullptr);
    mGuiManager = new GUIManager(mWindow, root);

    root->addChild(new AnalysisWindow(ANALYSIS_WINDOW_POS, root));
    root->addChild(new VisualSortWindow(VISUAL_SORT_WINDOW_POS, root));
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
        mWindow.clear(LGL::Color(0.8f, 0.8f, 0.2f));
        mGuiManager->draw();
        mWindow.display();
    }
}