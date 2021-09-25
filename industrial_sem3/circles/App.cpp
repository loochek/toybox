#include <stdexcept>
#include "App.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

const int CIRCLES_COUNT = 26;
const int SQUARES_COUNT = 25;

App::App() : graphics(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), physicalWorld(Vec2f(WINDOW_WIDTH, WINDOW_HEIGHT))
{
    circles = new Circle[CIRCLES_COUNT];
    squares = new Square[SQUARES_COUNT];

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            circles[i * 5 + j] = Circle(Vec2f(100.0f + 30.0f * i, 100.f + 30.0f * j),
                                        10.0f, 1.0f, Vec2f(0.0f, 0.0f), Color(1.0f, 1.0f, 0.0f));
        }
    }

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            squares[i * 5 + j] = Square(Vec2f(500.0f + 30.0f * i, 500.f + 30.0f * j),
                                        20.0f, 1.0f, Vec2f(0.0f, 0.0f), Color(1.0f, 0.0f, 1.0f));
        }
    }

    circles[25] = Circle(Vec2f(400.0f, 100.0f), 60.0f, 1.2f, Vec2f(700.0f, 30.0f));

    for (int i = 0; i < CIRCLES_COUNT; i++)
        physicalWorld.addObject(&circles[i]);

    for (int i = 0; i < SQUARES_COUNT; i++)
        physicalWorld.addObject(&squares[i]);
}

App::~App()
{
    delete[] circles;
    delete[] squares;
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
            graphics.draw(circles[i]);

        for (int i = 0; i < SQUARES_COUNT; i++)
            graphics.draw(squares[i]);

        graphics.display();
    }
}