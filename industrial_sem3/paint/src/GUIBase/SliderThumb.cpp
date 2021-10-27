#include "SliderThumb.hpp"
#include "Slider.hpp"

SliderThumb::SliderThumb(const IntRect &widgetRect, int leftLimit, int rightLimit, Slider *parent) :
    Widget(widgetRect, parent), mLeftLimit(leftLimit), mRightLimit(rightLimit - widgetRect.size.x),
    mMousePressed(false)
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
    if (mMousePressed && mRect.position.x >= mLeftLimit && mRect.position.x <= mRightLimit)
    {
        move(Vec2i(globalMousePos.x - mOldMousePosition.x, 0));
        ((Slider*)mParent)->thumbMoved(mRect.position.x);
    }

    if (mRect.position.x < mLeftLimit)
        mRect.position.x = mLeftLimit;

    if (mRect.position.x > mRightLimit)
        mRect.position.x = mRightLimit;

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