#include "App.hpp"
#include "GUI/Button.hpp"
#include "GUI/CirclesWidget.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : graphics(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), guiManager(graphics)
{
    Button *button1 = new Button(Rect(Vec2f(1100.0f, 50.0f), Vec2f(100.0f, 50.0f)));
    Button *button2 = new Button(Rect(Vec2f(1100.0f, 120.0f), Vec2f(100.0f, 50.0f)));

    CirclesWidget *circles = new CirclesWidget(Rect(Vec2f(50.0f, 50.0f), Vec2f(1000, 500)),
                                               graphics);

    button1->setDelegate(circles);
    button2->setDelegate(circles);

    button1->setLabel("Add circle");
    button2->setLabel("Add square");

    circles->addCircleButton = button1;
    circles->addSquareButton = button2;

    guiManager.addWidget(button1);
    guiManager.addWidget(button2);
    guiManager.addWidget(circles);
}

App::~App()
{
}

void App::run()
{
    while (!graphics.shouldClose())
    {
        // Update
        
        float elapsedTime = graphics.timerReset();

        guiManager.update(elapsedTime, graphics.getWindowMousePosition(), graphics.isLeftMouseButtonPressed());

        // Draw

        graphics.clear();
        guiManager.draw();
        graphics.display();
    }
}