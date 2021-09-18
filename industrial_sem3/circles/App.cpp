#include <stdexcept>
#include "App.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

const int CIRCLES_COUNT = 5;


App::App() : graphics(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), physicalWorld(Vec2f(1280, 720))
{
    circles = new Circle[CIRCLES_COUNT];
    circles[0] = Circle(Vec2f(100.0f, 100.0f), 50.0f, Vec2f(10.0f, 1.0f));
    circles[1] = Circle(Vec2f(100.0f, 200.0f), 50.0f, Vec2f(7.0f, 6.0f));
    circles[2] = Circle(Vec2f(100.0f, 300.0f), 50.0f, Vec2f(5.0f, 6.0f));
    circles[3] = Circle(Vec2f(100.0f, 400.0f), 50.0f, Vec2f(2.0f, 12.0f));
    circles[4] = Circle(Vec2f(100.0f, 500.0f), 50.0f, Vec2f(5.0f, 5.0f));

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
        
        physicalWorld.update(0.01f);

        // Draw

        graphics.clear();

        for (int i = 0; i < CIRCLES_COUNT; i++)
            circles[i].draw(graphics);

        graphics.display();
    }
}