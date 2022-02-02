#include "App.hpp"
#include "TextureManager.hpp"
#include "BaseGUI/PrettyButton.hpp"
#include "BaseGUI/WindowManager.hpp"
#include "SortingWidgets/ChartWidget.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : mWindow(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT), "amogus", false)
{
    LGL::RenderTarget::loadFont();
    TextureManager::getInstance()->load();
    
    // GUI initialization

    WindowManager *root = new WindowManager(IntRect(Vec2i(), Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), nullptr);
    mGuiManager = new GUIManager(mWindow, root);

    chart1.addValue(2);
    chart1.addValue(1);
    chart1.addValue(3);
    chart2.addValue(3);
    chart2.addValue(1);
    chart2.addValue(2);

    ChartWidget *chart = new ChartWidget(IntRect(Vec2i(100, 100), Vec2i(300, 150)));
    root->addChild(chart);

    chart->addChart(&chart1, LGL::Color::Red);
    chart->addChart(&chart2, LGL::Color::Magenta);
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