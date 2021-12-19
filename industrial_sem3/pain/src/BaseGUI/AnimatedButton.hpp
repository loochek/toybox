#ifndef ANIMATED_BUTTON_HPP
#define ANIMATED_BUTTON_HPP

#include "BaseButton.hpp"

class Label;

/**
 * Base class for animated button
 */
class AnimatedButton : public BaseButton
{
public:
    /**
     * Sets a label for the button. 
     * No label will drawn if null pointer is passed
     * 
     * \param label Label text
     */
    virtual void setLabel(const char *label);
    
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

    Label *mLabel;

    static const float ANIM_TIME;
};

#endif