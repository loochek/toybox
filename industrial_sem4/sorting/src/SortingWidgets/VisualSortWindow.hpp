#ifndef VISUAL_SORT_WINDOW_HPP
#define VISUAL_SORT_WINDOW_HPP

#include "../BaseGUI/Window.hpp"
#include "../Sorting/VisualSortObject.hpp"

class VisualSortWidget;
class VisualSortWindowController;

class VisualSortWindow : public Window
{
public:
    VisualSortWindow() = delete;
    VisualSortWindow(const Vec2i &windowPos, Widget *parent = nullptr);

    virtual ~VisualSortWindow();

protected:
    virtual void onUpdateThis(float elapsedTime);

protected:
    VisualSortWidget *mVisualSort;
    
    VisualSortWindowController *mController;
    friend class VisualSortWindowController;
};

#endif