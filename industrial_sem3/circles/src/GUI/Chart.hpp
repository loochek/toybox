#ifndef CHART_HPP
#define CHART_HPP

#include "../Graphics.hpp"
#include "RectangularWidget.hpp"

class Chart : public RectangularWidget
{
public:
    Chart() = delete;
    Chart(const Rect &rect);

    ~Chart();

    void update(float elapsedTime) override;
    void draw(Graphics &graphics) override;

    /**
     * Sets graph value
     * 
     * \param graph Graph ID
     * \param value Value
     */
    void setValue(int graph, int value);

    /**
     * Sets graph color
     * 
     * \param graph Graph ID
     * \param color Color
     */
    void setColor(int graph, const Color &color);

private:
    int *currValues;
    int valuesCounter;

    Color *colors;

    int **values;
    int maxValue;

    float timer;
};

#endif