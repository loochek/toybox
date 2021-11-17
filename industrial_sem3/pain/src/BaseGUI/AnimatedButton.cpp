#include "AnimatedButton.hpp"
#include "../BaseGUILogic/BaseButton/ButtonDelegate.hpp"

const float AnimatedButton::ANIM_TIME = 0.4f;

AnimatedButton::AnimatedButton(const IntRect &widgetRect, Widget *parent) : 
    BaseButton(widgetRect, parent), mButtonHovered(false), mButtonPressed(false), mCurrAnimTime(0.0f)
{
}

void AnimatedButton::onUpdateThis(float elapsedTime)
{
    if (mButtonHovered)
    {
        if (mCurrAnimTime < ANIM_TIME)
            mCurrAnimTime = std::min(mCurrAnimTime + elapsedTime, ANIM_TIME);
    }
    else
    {
        if (mCurrAnimTime > 0.0f)
            mCurrAnimTime = std::max(mCurrAnimTime - elapsedTime, 0.0f);
    }
}

void AnimatedButton::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mButtonHovered = true;
}

void AnimatedButton::onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    onPress();
}

void AnimatedButton::onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    onRelease();
}

void AnimatedButton::onMouseHoverEnd()
{
    mButtonHovered = false;
}

EventResult AnimatedButton::onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    if (key == LGL::KeyboardKey::Enter)
    {
        onPress();
        return EventResult::Handled;
    }
    
    return EventResult::Ignored;
}

EventResult AnimatedButton::onKeyReleasedThis(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    if (key == LGL::KeyboardKey::Enter)
    {
        onRelease();
        return EventResult::Handled;
    }
    
    return EventResult::Ignored;
}

void AnimatedButton::onPress()
{
    mButtonPressed = true;
    mCurrAnimTime = ANIM_TIME;

    if (mDelegate != nullptr)
        mDelegate->onClick(mUserData);
}

void AnimatedButton::onRelease()
{
    mButtonPressed = false;
}