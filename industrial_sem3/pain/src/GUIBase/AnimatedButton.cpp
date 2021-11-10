#include "AnimatedButton.hpp"

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

void AnimatedButton::onMouseClicked()
{
    mButtonPressed = true;
    mCurrAnimTime = ANIM_TIME;

    if (mDelegate != nullptr)
        mDelegate->onClick(mUserData);
}

void AnimatedButton::onMouseReleased()
{
    mButtonPressed = false;
}

void AnimatedButton::onMouseHoverEnd()
{
    mButtonHovered = false;
}

EventResult AnimatedButton::onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    if (key == LGL::KeyboardKey::Enter)
        onMouseClicked();

    return EventResult::Handled;
}

EventResult AnimatedButton::onKeyReleasedThis(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    if (key == LGL::KeyboardKey::Enter)
        onMouseReleased();

    return EventResult::Handled;
}