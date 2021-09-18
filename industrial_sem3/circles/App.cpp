#include <stdexcept>
#include "App.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

const int CIRCLES_COUNT = 25;


App::App() : graphics(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), physicalWorld(Vec2f(1280, 720))
{
    circles = new Circle[CIRCLES_COUNT];
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
            circles[i * 5 + j] = Circle(Vec2f(100.0f + 30.0f * i, 100.f + 30.0f * j), 10.0f, Vec2f(120.0f, 110.0f));
    }

    for (int i = 0; i < CIRCLES_COUNT; i++)
        physicalWorld.addObject(&circles[i]);
}

App::~App()
{
    delete[] circles;
}

void App::run()
{
    while (!graphics.shouldClose())
    {
        // Update

        float elapsedTime = graphics.timerReset();
        
        for (int i = 0; i < 10; i++)
            physicalWorld.update(elapsedTime / 10.0f);

        // Draw

        graphics.clear();

        for (int i = 0; i < CIRCLES_COUNT; i++)
            circles[i].draw(graphics);

        graphics.display();
    }
}