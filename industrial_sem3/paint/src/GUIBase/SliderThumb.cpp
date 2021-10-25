#include "SliderThumb.hpp"

SliderThumb::SliderThumb(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent), mMousePressed(false)
{
}

void SliderThumb::onRedrawThis()
{
    mTexture.drawRect(IntRect(Vec2i(), mRect.size));
}

void SliderThumb::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mOldMousePosition = globalMousePos;
}

void SliderThumb::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    if (mMousePressed)
        move(Vec2i(globalMousePos.x - mOldMousePosition.x, 0));

    mOldMousePosition = globalMousePos;
}

void SliderThumb::onMouseClicked()
{
    mMousePressed = true;
}

void SliderThumb::onMouseReleased()
{
    mMousePressed = false;
}