#include "DragArea.hpp"

DragArea::DragArea(const IntRect &widgetRect, Widget *widgetToDrag, Widget *parent) :
    Widget(widgetRect, parent), mWidgetToDrag(widgetToDrag), mMousePressed(false)
{
}

void DragArea::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mOldMousePosition = globalMousePos;
}

void DragArea::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    // Using global mouse position because of self-moving

    if (mMousePressed)
        mWidgetToDrag->move(globalMousePos - mOldMousePosition);
    
    mOldMousePosition = globalMousePos;
}

void DragArea::onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mMousePressed = true;
}

void DragArea::onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mMousePressed = false;
}