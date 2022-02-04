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

    void setArray(std::vector<VisualSortObject> *array) { mArray = array; };
    void highlight(int idx1, int idx2) { mHighlighIdx1 = idx1; mHighlighIdx2 = idx2; };

protected:
    virtual void onRedrawThis() override;

protected:
    std::vector<VisualSortObject> *mArray;

    int mHighlighIdx1, mHighlighIdx2;
};

#endif