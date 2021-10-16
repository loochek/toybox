#include "App.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : window(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), rt(Vec2i(500.0f, 500.0f))
{
}

App::~App()
{
}

void App::run()
{
    while (!window.shouldClose())
    {
        // Update
        
        float elapsedTime = window.timerReset();

        rt.clear(LGL::Color(1.0f, 1.0f, 0.0f));
        rt.drawCircle(Vec2f(50.0f, 50.0f), 50.0f);

        // Draw

        window.clear();
        window.drawRenderTexture(rt, Vec2f(100.0f, 100.0f), Vec2f(-100.0f, -100.0f));
        window.display();
    }
}