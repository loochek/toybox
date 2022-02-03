#include "ChartWidget.hpp"

const int TOP_LIMIT_OFFS = 10;

const int START_X_OFFS = 10;
const int START_Y_OFFS = 10;

const float AXIS_THICKNESS        = 1.0f;
const float CHART_POINT_THICKNESS = 1.0f;
const float CHART_LINE_THICKNESS  = 1.0f;

ChartWidget::ChartWidget(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent), mChartTexture(widgetRect.size)
{
}

void ChartWidget::putChart(const BaseChart *chart, const LGL::Color &color)
{
    if (std::find(mCharts.begin(), mCharts.end(), chart) == mCharts.end())
        mCharts.push_back(chart);

    mColors[chart] = color;
    redrawCharts();
}

void ChartWidget::redrawCharts()
{
    mChartTexture.clear();

    // Determine max param and value
    int maxParam = -1;
    int maxValue = -1;
    for (const BaseChart *chart : mCharts)
    {
        int valuesCount = chart->getValuesCount();
        maxParam = std::max(maxParam, valuesCount);

        for (int i = 0; i < valuesCount; i++)
            maxValue = std::max(maxValue, (*chart)[i]);
    }

    int chartWidth  = mRect.size.x - START_X_OFFS;
    int chartHeight = mRect.size.y - START_Y_OFFS - TOP_LIMIT_OFFS;

    float paramStep = (float)chartWidth / (maxParam - 1);
    float valueStep = (float)chartHeight / maxValue;

    // Draw each chart
    for (const BaseChart *chart : mCharts)
    {
        int valuesCount = chart->getValuesCount();

        Vec2f prevPoint(-1, -1);

        for (int i = 0; i < valuesCount; i++)
        {
            float pointWidth  = START_X_OFFS + paramStep * i;
            float pointHeight = mRect.size.y - START_Y_OFFS - valueStep * (*chart)[i];
            Vec2f currPoint(pointWidth, pointHeight);

            mChartTexture.drawCircle(currPoint, CHART_POINT_THICKNESS, mColors[chart]);
            if (prevPoint.x >= 0)
                mChartTexture.drawLine(prevPoint, currPoint, CHART_LINE_THICKNESS, mColors[chart]);
                
            prevPoint = currPoint;
        }
    }
}

void ChartWidget::onRedrawThis()
{
    mTexture.clear(LGL::Color::White);
    
    // Draw axis
    mChartTexture.drawLine(Vec2i(START_X_OFFS, 0), Vec2i(START_X_OFFS, mRect.size.y),
                      AXIS_THICKNESS, LGL::Color::Black);
    mChartTexture.drawLine(Vec2i(0, mRect.size.y - START_Y_OFFS), Vec2i(mRect.size.x, mRect.size.y - START_Y_OFFS),
                      AXIS_THICKNESS, LGL::Color::Black);

    // Draw charts
    mTexture.drawRenderTexture(mChartTexture, Vec2i());
}