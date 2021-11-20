#ifndef SPLINE_WIDGET_HPP
#define SPLINE_WIDGET_HPP

#include <vector>
#include "../Utils/CatmullRomSpline.hpp"
#include "../BaseGUI/Widget.hpp"

class SplineWidget : public Widget
{
public:
    SplineWidget(const IntRect &widgetRect, Widget *parent = nullptr);

protected:
    virtual void onRedrawThis() override;
    virtual void onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;

private:
    void recalcSpline();

private:
    CatmullRomSpline mSpline;

    // to call recalcSpline
    friend class SplinePoint;
};

#endif