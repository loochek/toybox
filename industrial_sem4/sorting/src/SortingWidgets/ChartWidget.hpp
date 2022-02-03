#ifndef CHART_WIDGET_HPP
#define CHART_WIDGET_HPP

#include <vector>
#include "../BaseGUI/Widget.hpp"
#include "../Utils/BaseChart.hpp"

class ChartWidget : public Widget
{
public:
    ChartWidget() = delete;
    ChartWidget(const IntRect &widgetRect, Widget *parent = nullptr);

    /**
     * Adds chart to the widget with specified color and redraw all charts.
     * If given chart already present, only changes the color
     */
    void putChart(const BaseChart *chart, const LGL::Color &color = LGL::Color::Black);

    /**
     * Redraw all charts
     */
    void redrawCharts();

protected:
    virtual void onRedrawThis() override;

protected:
    std::vector<const BaseChart*> mCharts;
    std::map<const BaseChart*, LGL::Color> mColors;

    LGL::RenderTexture mChartTexture;
};

#endif