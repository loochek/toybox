#ifndef DRAG_AREA_HPP
#define DRAG_AREA_HPP

#include "Widget.hpp"

/**
 * Helper widget to drag something
 */
class DragArea : public Widget
{
public:
    DragArea() = delete;
    DragArea(const IntRect &widgetRect, Widget *widgetToDrag, Widget *parent = nullptr);

protected:
    virtual void onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;

protected:
    bool mMousePressed;
    Vec2i mOldMousePosition;

    Widget *mWidgetToDrag;
};

#endif