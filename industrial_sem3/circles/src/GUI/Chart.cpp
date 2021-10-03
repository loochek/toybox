#include <cassert>
#include "Chart.hpp"

const Color BACKGROUND_COLOR    = Color(0.2f, 0.2f, 0.2f);
const Color GRAPH_DEFAULT_COLOR = Color(1.0f, 1.0f, 1.0f);

const int MAX_MEASUREMENTS_COUNT = 1000;
const int MAX_GRAPHS_COUNT = 2;
const int INVARIANT_VALUE = -1;
const int INITIAL_MAX_VALUE = 1;

const float GRAPH_THICKNESS = 1.0f;

const float TIME_STEP = 0.05f;

Chart::Chart(const Rect &rect) : RectangularWidget(rect), valuesCounter(0), maxValue(INITIAL_MAX_VALUE),
                                 timer(0.0f)
{
    values = new int*[MAX_GRAPHS_COUNT];
    for (int graph = 0; graph < MAX_GRAPHS_COUNT; graph++)
    {
        values[graph] = new int[MAX_MEASUREMENTS_COUNT];

        for (int i = 0; i < MAX_MEASUREMENTS_COUNT; i++)
            values[graph][i] = INVARIANT_VALUE;
    }

    currValues = new int[MAX_MEASUREMENTS_COUNT];
    for (int graph = 0; graph < MAX_GRAPHS_COUNT; graph++)
        currValues[graph] = INVARIANT_VALUE;

    colors = new Color[MAX_GRAPHS_COUNT];
    for (int graph = 0; graph < MAX_GRAPHS_COUNT; graph++)
        colors[graph] = GRAPH_DEFAULT_COLOR;
}

Chart::~Chart()
{
    for (int graph = 0; graph < MAX_GRAPHS_COUNT; graph++)
        delete[] values[graph];

    delete[] values;
    delete[] currValues;
    delete[] colors;
}

void Chart::update(float elapsedTime)
{
    timer += elapsedTime;
    if (timer >= TIME_STEP)
    {
        timer = 0.0f;

        for (int graph = 0; graph < MAX_GRAPHS_COUNT; graph++)
            values[graph][valuesCounter] = currValues[graph];

        valuesCounter = (valuesCounter + 1) % MAX_MEASUREMENTS_COUNT;
    }
}

void Chart::draw(Graphics &graphics)
{
    graphics.drawRect(rect, BACKGROUND_COLOR);

    Vec2f graphOrigin = rect.position + Vec2f(0.0f, rect.size.y);

    for (int graph = 0; graph < MAX_GRAPHS_COUNT; graph++)
    {
        Vec2f prevPoint;
        for (int i = 0; i < MAX_MEASUREMENTS_COUNT; i++)
        {
            float dotOffset = (float)i / MAX_MEASUREMENTS_COUNT * rect.size.x;

            if (values[graph][i] != INVARIANT_VALUE)
            {
                float dotHeight = (float)values[graph][i] / maxValue * rect.size.y;
                Vec2f currPoint = graphOrigin + Vec2f(dotOffset, -dotHeight);

                graphics.drawCircle(currPoint, GRAPH_THICKNESS, colors[graph]);

                if (i != 0 && i != valuesCounter)
                    graphics.drawLine(prevPoint, currPoint, GRAPH_THICKNESS, colors[graph]);

                prevPoint = currPoint;
            }
        }
    }
}

void Chart::setValue(int graph, int value)
{
    assert(graph < MAX_GRAPHS_COUNT);

    currValues[graph] = value;
    if (currValues[graph] > maxValue)
        maxValue = currValues[graph];
}

void Chart::setColor(int graph, const Color &color)
{
    assert(graph < MAX_GRAPHS_COUNT);

    colors[graph] = color;
}