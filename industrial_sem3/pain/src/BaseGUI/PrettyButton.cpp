#include "../TextureManager.hpp"
#include "Label.hpp"
#include "PrettyButton.hpp"

const int PrettyButton::BUTTON_HEIGHT = 22;
const int SIDE_WIDTH = 3;

PrettyButton::PrettyButton(const Vec2i &buttonPos, int buttonSize, Widget *parent) : 
    AnimatedButton(IntRect(buttonPos, Vec2i(buttonSize, BUTTON_HEIGHT)), parent)
{
    getTextures();
}

void PrettyButton::onRedrawThis()
{
    Vec2i leftPos, bodyPos(SIDE_WIDTH, 0), rightPos(Vec2i(mRect.size.x - SIDE_WIDTH, 0));
    IntRect bodyRect(Vec2i(), Vec2i(mRect.size.x, BUTTON_HEIGHT));

    if (mButtonPressed)
    {
        // Left
        mTexture.drawTexture(*mLeftPressedTexture, leftPos);
        // Body
        mTexture.drawTexture(*mBodyPressedTexture, bodyPos, bodyRect);
        // Right
        mTexture.drawTexture(*mRightPressedTexture, rightPos);
    }
    else
    {
        // Draw idle texture

        mTexture.drawTexture(*mLeftIdleTexture, leftPos);
        mTexture.drawTexture(*mBodyIdleTexture, bodyPos, bodyRect);
        mTexture.drawTexture(*mRightIdleTexture, rightPos);

        // Draw hovered texture over idle texture

        IntRect defaultRect(Vec2i(), Vec2i(-1, -1));
        float opacity = mCurrAnimTime / ANIM_TIME;
        mTexture.drawTexture(*mLeftHoveredTexture, leftPos , defaultRect, opacity);
        mTexture.drawTexture(*mBodyHoveredTexture, bodyPos , bodyRect   , opacity);
        mTexture.drawTexture(*mRightHoveredTexture, rightPos, defaultRect, opacity);
    }    
}

void PrettyButton::setLabel(const char *text)
{
    AnimatedButton::setLabel(text);
    // Center label
    mLabel->setPosition(Vec2i(mRect.size.x / 2 - mLabel->getRect().size.x / 2, mLabel->getRect().position.y));
}

void PrettyButton::getTextures()
{
    mLeftIdleTexture    = TextureManager::getInstance()->getTexture("button_left_idle");
    mLeftHoveredTexture = TextureManager::getInstance()->getTexture("button_left_hovered");
    mLeftPressedTexture = TextureManager::getInstance()->getTexture("button_left_pressed");

    mBodyIdleTexture    = TextureManager::getInstance()->getTexture("button_body_idle");
    mBodyHoveredTexture = TextureManager::getInstance()->getTexture("button_body_hovered");
    mBodyPressedTexture = TextureManager::getInstance()->getTexture("button_body_pressed");

    mRightIdleTexture    = TextureManager::getInstance()->getTexture("button_right_idle");
    mRightHoveredTexture = TextureManager::getInstance()->getTexture("button_right_hovered");
    mRightPressedTexture = TextureManager::getInstance()->getTexture("button_right_pressed");

    if (mLeftIdleTexture == nullptr ||
        mLeftHoveredTexture == nullptr ||
        mLeftPressedTexture == nullptr ||
        mBodyIdleTexture == nullptr ||
        mBodyHoveredTexture == nullptr ||
        mBodyPressedTexture == nullptr ||
        mRightIdleTexture == nullptr ||
        mRightHoveredTexture == nullptr ||
        mRightPressedTexture == nullptr)
    {
        throw std::runtime_error("Slider textures are not loaded");
    }
}
