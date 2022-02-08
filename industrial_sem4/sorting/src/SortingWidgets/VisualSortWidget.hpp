#ifndef VISUAL_SORT_WIDGET_HPP
#define VISUAL_SORT_WIDGET_HPP

#include <vector>
#include "../BaseGUI/Widget.hpp"

class VisualSortObject;

class VisualSortWidget : public Widget
{
public:
    VisualSortWidget() = delete;
    VisualSortWidget(const IntRect &widgetRect, Widget *parent = nullptr);

    void redraw(const std::vector<VisualSortObject> &array, int highlIdx1, int highlIdx2);

protected:
    virtual void onRedrawThis() override;

protected:
    LGL::RenderTexture mSortPicture;
};

#endif