#include "../TextureManager.hpp"
#include "SliderThumb.hpp"
#include "Slider.hpp"

const int THUMB_HEIGHT      = 16;
const int THUMB_SIDE_SIZE   = 4;
const int THUMB_CENTER_SIZE = 10;

SliderThumb::SliderThumb(const Vec2i &thumbPos, int thumbSize, int leftLimit, int rightLimit, Slider *parent) :
    Widget(IntRect(thumbPos, Vec2i(thumbSize, THUMB_HEIGHT)), parent), mMousePressed(false),
    mLeftLimit(leftLimit), mRightLimit(rightLimit - thumbSize)
{
    getTextures();
}

void SliderThumb::resize(int newThumbSize)
{
    mRightLimit -= newThumbSize - mRect.size.x;
    mRect.size = Vec2i(newThumbSize, THUMB_HEIGHT);
    mTexture.resize(mRect.size);
}

void SliderThumb::onRedrawThis()
{
    // Left
    mTexture.drawTexture(*mThumbLeftTexture, Vec2i());
    // Right
    mTexture.drawTexture(*mThumbRightTexture, Vec2i(mRect.size.x - THUMB_SIDE_SIZE, 0));
    // Body
    mTexture.drawTexture(*mThumbBodyTexture,
                         Vec2i(THUMB_SIDE_SIZE, 0),
                         IntRect(Vec2i(), Vec2i(mRect.size.x - 2 * THUMB_SIDE_SIZE, THUMB_HEIGHT)));
    // Center
    mTexture.drawTexture(*mThumbCenterTexture, Vec2i(mRect.size.x / 2 - THUMB_CENTER_SIZE / 2, 0));
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

        if (mRect.position.x < mLeftLimit)
            mRect.position.x = mLeftLimit;

        if (mRect.position.x > mRightLimit)
            mRect.position.x = mRightLimit;

        ((Slider*)mParent)->thumbMoved(mRect.position.x);
    }

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

void SliderThumb::getTextures()
{
    mThumbLeftTexture   = TextureManager::getInstance()->getTexture("slider_thumb_left");
    mThumbRightTexture  = TextureManager::getInstance()->getTexture("slider_thumb_right");
    mThumbBodyTexture   = TextureManager::getInstance()->getTexture("slider_thumb_body");
    mThumbCenterTexture = TextureManager::getInstance()->getTexture("slider_thumb_center");

    if (mThumbLeftTexture == nullptr ||
        mThumbRightTexture == nullptr ||
        mThumbBodyTexture == nullptr ||
        mThumbCenterTexture == nullptr)
    {
        throw std::runtime_error("Slider thumb textures are not loaded");
    }
}