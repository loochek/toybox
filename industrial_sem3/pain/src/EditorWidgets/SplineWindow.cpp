#include "SplineWidget.hpp"
#include "../BaseGUI/BaseButton.hpp"
#include "../BaseGUILogic/Window/WindowCloseDelegate.hpp"
#include "SplineWindow.hpp"

const int POINT_COUNT = 20;

SplineWindow::SplineWindow(const IntRect &splineWidgetRect, Widget *parent) :
    Window(splineWidgetRect, parent)
{
    setTitle("Catmull-Rom spline");

    mSplineWidget = new SplineWidget(IntRect(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT),
                                             splineWidgetRect.size),
                                     this);
    addChild(mSplineWidget);

    mCloseButtonDelegate = new WindowCloseDelegate(this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

SplineWindow::~SplineWindow()
{
    delete mCloseButtonDelegate;
}