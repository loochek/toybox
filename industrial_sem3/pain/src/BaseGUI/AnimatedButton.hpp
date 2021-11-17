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
    virtual void onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseHoverEnd() override;

    virtual void onUpdateThis(float elapsedTime) override;
    virtual EventResult onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier) override;
    virtual EventResult onKeyReleasedThis(LGL::KeyboardKey key, LGL::InputModifier modifier) override;

private:
    void onPress();
    void onRelease();

protected:
    bool  mButtonHovered;
    bool  mButtonPressed;
    float mCurrAnimTime;

    static const float ANIM_TIME;
};

#endif