#include "MainColorPicker.hpp"
#include "../EditorLogic/ColorChangeDelegate.hpp"
#include "../TextureManager.hpp"

MainColorPicker::MainColorPicker(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent), mDelegate(nullptr)
{
    mSelectorTexture = TextureManager::getInstance()->getTexture("color_picker_selector");
    if (mSelectorTexture == nullptr)
        throw std::runtime_error("Color picker selector texture is not loaded");

    mGradientImage.create(widgetRect.size.x, widgetRect.size.y, LGL::Color::Transparent);
    redrawGradient();
}

void MainColorPicker::setKeyColor(const LGL::Color &color)
{
    mKeyColor = color;
    redrawGradient();
    
    if (mDelegate != nullptr)
        mDelegate->onColorChange(calcColor(mSelectorOffs), mUserData);
};

void MainColorPicker::onRedrawThis()
{
    mTexture.drawTexture(mGradientTexture, Vec2i());
    mTexture.drawTexture(*mSelectorTexture, mSelectorOffs - mSelectorTexture->getSize() / 2);
}

void MainColorPicker::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mMouseOffs = localMousePos;
}

void MainColorPicker::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mMouseOffs = localMousePos;
    if (mMousePressed)
    {
        mSelectorOffs = mMouseOffs;

        if (mSelectorOffs.y > mRect.size.y)
            mSelectorOffs.y = mRect.size.y;

        if (mSelectorOffs.y < 0)
            mSelectorOffs.y = 0;

        if (mSelectorOffs.x > mRect.size.x)
            mSelectorOffs.x = mRect.size.x;

        if (mSelectorOffs.x < 0)
            mSelectorOffs.x = 0;

        if (mDelegate != nullptr)
            mDelegate->onColorChange(calcColor(mSelectorOffs), mUserData);
    }
}

void MainColorPicker::onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mMousePressed = true;
    mSelectorOffs = mMouseOffs;

    if (mDelegate != nullptr)
        mDelegate->onColorChange(calcColor(mSelectorOffs), mUserData);
}

void MainColorPicker::onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mMousePressed = false;
}

void MainColorPicker::redrawGradient()
{
    for (int y = 0; y < mRect.size.y; y++)
    {
        for (int x = 0; x < mRect.size.x; x++)
        {
            mGradientImage.setPixel(x, y, calcColor(Vec2i(x, y)));
        }
    }

    mGradientTexture.loadFromImage(mGradientImage);
}

LGL::Color MainColorPicker::calcColor(Vec2i offset)
{
    float percPosX = (float)offset.x / mRect.size.x;
    float percPosY = (float)offset.y / mRect.size.y;

    return LGL::Color(
        (1.0f - percPosY) * (mKeyColor.r + (1.0f - percPosX) * (1.0f - mKeyColor.r)),
        (1.0f - percPosY) * (mKeyColor.g + (1.0f - percPosX) * (1.0f - mKeyColor.g)),
        (1.0f - percPosY) * (mKeyColor.b + (1.0f - percPosX) * (1.0f - mKeyColor.b))
    );
}