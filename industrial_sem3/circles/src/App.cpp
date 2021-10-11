#include "App.hpp"
#include "GUI/Button.hpp"
#include "GUI/CirclesWidget.hpp"
#include "GUI/Chart.hpp"
#include "GUI/Controller.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

const int CIRCLES_GRAPH_ID = 0;
const int SQUARES_GRAPH_ID = 1;

const Vec2f ADD_CIRCLE_BUTTON_POSITION   = Vec2f(1100.0f, 50.0f);
const Vec2f ADD_SQUARE_BUTTON_POSITION   = Vec2f(1100.0f, 120.0f);
const Vec2f ADD_TRIANGLE_BUTTON_POSITION = Vec2f(1100.0f, 190.0f);
const Vec2f BUTTON_SIZE                  = Vec2f(100.0f, 50.0f);

const Vec2f CIRCLES_WIDGET_POSITION = Vec2f(50.0f, 50.0f);
const Vec2f CIRCLES_WIDGET_SIZE     = Vec2f(1000.0f, 500.0f);

const Vec2f CHART_POSITION = Vec2f(50.0f, 560.0f);
const Vec2f CHART_SIZE     = Vec2f(1000.0f, 100.0f);

const Color CIRCLES_GRAPH_COLOR = Color(0.0f, 1.0f, 1.0f);
const Color SQUARES_GRAPH_COLOR = Color(0.0f, 1.0f, 0.0f);


App::App() : graphics(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), guiManager(graphics)
{
    Button *addCircleButton   = new Button(Rect(ADD_CIRCLE_BUTTON_POSITION  , BUTTON_SIZE));
    Button *addSquareButton   = new Button(Rect(ADD_SQUARE_BUTTON_POSITION  , BUTTON_SIZE));
    Button *addTriangleButton = new Button(Rect(ADD_TRIANGLE_BUTTON_POSITION, BUTTON_SIZE));

    circlesWidget = new CirclesWidget(Rect(CIRCLES_WIDGET_POSITION, CIRCLES_WIDGET_SIZE), graphics);

    chart = new Chart(Rect(CHART_POSITION, CHART_SIZE));

    addCircleButton->setDelegate(new AddObjectDelegate(circlesWidget, &CirclesWidget::createCircle,
                                                       CIRCLE_SIZE, CIRCLE_COLOR));

    addSquareButton->setDelegate(new AddObjectDelegate(circlesWidget, &CirclesWidget::createSquare,
                                                       SQUARE_SIZE, SQUARE_COLOR));

    addTriangleButton->setDelegate(new AddObjectDelegate(circlesWidget, &CirclesWidget::createTriangle,
                                                         TRIANGLE_SIZE, TRIANGLE_COLOR));

    addCircleButton->setLabel("Add circle");
    addSquareButton->setLabel("Add square");
    addTriangleButton->setLabel("Add triangle");

    chart->setColor(CIRCLES_GRAPH_ID, CIRCLES_GRAPH_COLOR);
    chart->setColor(SQUARES_GRAPH_ID, SQUARES_GRAPH_COLOR);

    guiManager.addWidget(addCircleButton);
    guiManager.addWidget(addSquareButton);
    guiManager.addWidget(addTriangleButton);
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
        
        guiManager.update(elapsedTime);

        // Draw

        graphics.clear();
        guiManager.draw();
        graphics.display();
    }
}