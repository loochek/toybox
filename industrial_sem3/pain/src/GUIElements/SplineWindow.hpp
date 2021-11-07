#ifndef SPLINE_WINDOW_HPP
#define SPLINE_WINDOW_HPP

#include "../GUIBase/Window.hpp"

class SplineWidget;
class WindowCloseDelegate;

class SplineWindow : public Window
{
public:
    SplineWindow() = delete;
    SplineWindow(const IntRect &splineWidgetRect, Widget *parent = nullptr);
    
    virtual ~SplineWindow();
    
    /**
     * \return Spline widget
     */
    SplineWidget *getSplineWidget() { return mSplineWidget; };

protected:
    SplineWidget *mSplineWidget;
    WindowCloseDelegate *mCloseButtonDelegate;
};

#endif