#include "MainColorPicker.hpp"

MainColorPicker::MainColorPicker(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent)
{
    mGradientTexture.create(widgetRect.size.x, widgetRect.size.y, LGL::Color::Transparent);
    redrawGradient();
}

void MainColorPicker::setKeyColor(const LGL::Color &color)
{
    mKeyColor = color;
    redrawGradient();
};

void MainColorPicker::onRedrawThis()
{
    mTexture.drawTexture(mGradientTexture, Vec2i());
}

// void MainColorPicker::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
// {
//     mMouseOffs = localMousePos.y;
//     if (mMousePressed)
//         mSelectorOffs = mMouseOffs;
// }

// void MainColorPicker::onMouseClicked()
// {
//     mMousePressed = true;
//     mSelectorOffs = mMouseOffs;
// }

// void MainColorPicker::onMouseReleased()
// {
//     mMousePressed = false;
// }

void MainColorPicker::redrawGradient()
{
    for (int y = 0; y < mRect.size.y; y++)
    {
        for (int x = 0; x < mRect.size.x; x++)
        {
            mGradientTexture.setPixel(x, y, calcColor(x, y));
        }
    }

    mGradientTexture.update();
}

LGL::Color MainColorPicker::calcColor(int positionX, int positionY)
{
    float percPosX = (float)positionX / mRect.size.x;
    float percPosY = (float)positionY / mRect.size.y;

    return LGL::Color(
        (1.0 - percPosY) * (mKeyColor.r + (1.0 - percPosX) * (1.0f - mKeyColor.r)),
        (1.0 - percPosY) * (mKeyColor.g + (1.0 - percPosX) * (1.0f - mKeyColor.g)),
        (1.0 - percPosY) * (mKeyColor.b + (1.0 - percPosX) * (1.0f - mKeyColor.b))
    );
}