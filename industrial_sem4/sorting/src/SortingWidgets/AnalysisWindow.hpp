#ifndef ANALYSIS_WINDOW_HPP
#define ANALYSIS_WINDOW_HPP

#include "../BaseGUI/Window.hpp"

class ChartWidget;
class AnalysisWindowController;

class AnalysisWindow : public Window
{
public:
    AnalysisWindow() = delete;
    AnalysisWindow(const Vec2i &windowPos, Widget *parent = nullptr);

    virtual ~AnalysisWindow();

protected:
    ChartWidget *mCompChart;
    
    AnalysisWindowController *mController;
    friend class AnalysisWindowController;
};

#endif