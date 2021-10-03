#include "App.hpp"
#include "GUI/Button.hpp"
#include "GUI/CirclesWidget.hpp"
#include "GUI/Chart.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

const int CIRCLES_GRAPH_ID = 0;
const int SQUARES_GRAPH_ID = 1;

const Vec2f CIRCLE_ADD_BUTTON_POSITION = Vec2f(1100.0f, 50.0f);
const Vec2f SQUARE_ADD_BUTTON_POSITION = Vec2f(1100.0f, 120.0f);
const Vec2f BUTTON_SIZE                = Vec2f(100.0f, 50.0f);

const Vec2f CIRCLES_WIDGET_POSITION = Vec2f(50.0f, 50.0f);
const Vec2f CIRCLES_WIDGET_SIZE     = Vec2f(1000.0f, 500.0f);

const Vec2f CHART_POSITION = Vec2f(50.0f, 560.0f);
const Vec2f CHART_SIZE     = Vec2f(1000.0f, 100.0f);

const Color CIRCLES_GRAPH_COLOR = Color(0.0f, 1.0f, 1.0f);
const Color SQUARES_GRAPH_COLOR = Color(0.0f, 1.0f, 0.0f);


App::App() : graphics(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), guiManager(graphics)
{
    Button *circleAddButton = new Button(Rect(CIRCLE_ADD_BUTTON_POSITION, BUTTON_SIZE));
    Button *squareAddButton = new Button(Rect(SQUARE_ADD_BUTTON_POSITION, BUTTON_SIZE));

    circlesWidget = new CirclesWidget(Rect(CIRCLES_WIDGET_POSITION, CIRCLES_WIDGET_SIZE), graphics);

    chart = new Chart(Rect(CHART_POSITION, CHART_SIZE));

    circleAddButton->setDelegate(circlesWidget);
    squareAddButton->setDelegate(circlesWidget);
    circleAddButton->setLabel("Add circle");
    squareAddButton->setLabel("Add square");

    circlesWidget->setCircleAddButton(circleAddButton);
    circlesWidget->setSquareAddButton(squareAddButton);

    chart->setColor(CIRCLES_GRAPH_ID, CIRCLES_GRAPH_COLOR);
    chart->setColor(SQUARES_GRAPH_ID, SQUARES_GRAPH_COLOR);

    guiManager.addWidget(circleAddButton);
    guiManager.addWidget(squareAddButton);
    guiManager.addWidget(circlesWidget);
    guiManager.addWidget(chart);
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

        chart->setValue(CIRCLES_GRAPH_ID, circlesWidget->getCirclesCount());
        chart->setValue(SQUARES_GRAPH_ID, circlesWidget->getSquaresCount());

        guiManager.update(elapsedTime, graphics.getWindowMousePosition(), graphics.isLeftMouseButtonPressed());

        // Draw

        graphics.clear();
        guiManager.draw();
        graphics.display();
    }
}