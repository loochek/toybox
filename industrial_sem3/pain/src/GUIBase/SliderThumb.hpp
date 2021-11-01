#ifndef SLIDER_THUMB_HPP
#define SLIDER_THUMB_HPP

#include "Widget.hpp"

class Slider;

class SliderThumb : public Widget
{
protected:
    SliderThumb() = delete;

    /**
     * \param leftLimit Thumb won't move left from this coordinate
     * \param rightLimit Thumb won't move right from this coordinate
     */
    SliderThumb(const IntRect &widgetRect, int leftLimit, int rightLimit, Slider *parent = nullptr);

    virtual void onRedrawThis() override;
    
    virtual void onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClicked() override;
    virtual void onMouseReleased() override;

protected:
    Vec2i mOldMousePosition;
    bool  mMousePressed;

    int mLeftLimit;
    int mRightLimit;

    friend class Slider;
};

#endif