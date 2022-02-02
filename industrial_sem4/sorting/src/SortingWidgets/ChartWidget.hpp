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

    void addChart(const BaseChart *chart, const LGL::Color &color = LGL::Color::Black);

protected:
    virtual void onRedrawThis() override;

protected:
    std::vector<const BaseChart*> mCharts;
    std::map<const BaseChart*, LGL::Color> mColors;
};

#endif