#include "AnalysisWindow.hpp"
#include "ChartWidget.hpp"
#include "../BaseGUI/PrettyButton.hpp"
#include "../SortingLogic/AnalysisWindow/AnalysisWindowController.hpp"

const Vec2i WINDOW_SIZE(630, 300);
const Vec2i COMP_CHART_POS(10, 10);
const Vec2i COMP_CHART_SIZE(610, 200);

const Vec2i BUTTON_POS(40, 250);
const int BUTTON_LENGTH = 100;

AnalysisWindow::AnalysisWindow(const Vec2i &windowPos, Widget *parent) :
    Window(IntRect(windowPos, WINDOW_SIZE), parent)
{
    mController = new AnalysisWindowController(this);

    IntRect compChartRect(COMP_CHART_POS + Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), COMP_CHART_SIZE);
    mCompChart = new ChartWidget(compChartRect, this);
    addChild(mCompChart);

    PrettyButton *button = new PrettyButton(BUTTON_POS, BUTTON_LENGTH, this);
    button->setLabel("std::sort");
    button->setDelegate(mController);
    addChild(button);
}

AnalysisWindow::~AnalysisWindow()
{
    delete mController;
}