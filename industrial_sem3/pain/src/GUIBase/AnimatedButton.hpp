#ifndef ANIMATED_BUTTON_HPP
#define ANIMATED_BUTTON_HPP

#include "BaseButton.hpp"

/**
 * Base class for animated button
 */
class AnimatedButton : public BaseButton
{
protected:
    AnimatedButton() = delete;
    AnimatedButton(const IntRect &widgetRect, Widget *parent = nullptr);

    virtual ~AnimatedButton() {};

    virtual void onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClicked() override;
    virtual void onMouseReleased() override;
    virtual void onMouseHoverEnd() override;

    virtual void onUpdateThis(float elapsedTime) override;
    virtual EventResult onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier) override;
    virtual EventResult onKeyReleasedThis(LGL::KeyboardKey key, LGL::InputModifier modifier) override;

protected:
    bool  mButtonHovered;
    bool  mButtonPressed;
    float mCurrAnimTime;

    static const float ANIM_TIME;
};

#endif