#ifndef SLIDER_THUMB_HPP
#define SLIDER_THUMB_HPP

#include "Widget.hpp"

class SliderThumb : public Widget
{
protected:
    SliderThumb() = delete;
    SliderThumb(const IntRect &widgetRect, Widget *parent = nullptr);

    virtual void onRedrawThis() override;
    
    virtual void onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClicked() override;
    virtual void onMouseReleased() override;

protected:
    Vec2i mOldMousePosition;
    bool  mMousePressed;

    friend class Slider;
};

#endif