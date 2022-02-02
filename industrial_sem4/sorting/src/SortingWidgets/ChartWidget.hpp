#ifndef CHART_WIDGET_HPP
#define CHART_WIDGET_HPP

#include <vector>
#include "../BaseGUI/Widget.hpp"
#include "../Utils/Chart.hpp"

class ChartWidget : public Widget
{
public:
    ChartWidget(const IntRect &widgetRect, Widget *parent = nullptr);

    void addChart(const Chart *chart, const LGL::Color &color = LGL::Color::Black);

protected:
    virtual void onRedrawThis() override;

protected:
    std::vector<const Chart*> mCharts;
    std::map<const Chart*, LGL::Color> mColors;
};

#endif