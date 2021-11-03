#include "KeyColorPicker.hpp"
#include "../GUILogic/KeyColorChangedDelegate.hpp"

const int SELECTOR_SIZE = 4;

const int BASE_COLORS_COUNT = 7;

const LGL::Color baseColors[BASE_COLORS_COUNT] =
{
    LGL::Color(0.0f, 0.0f, 0.0f),
    LGL::Color(1.0f, 0.0f, 1.0f),
    LGL::Color(0.0f, 0.0f, 1.0f),
    LGL::Color(0.0f, 1.0f, 1.0f),
    LGL::Color(0.0f, 1.0f, 0.0f),
    LGL::Color(1.0f, 1.0f, 0.0f),
    LGL::Color(1.0f, 0.0f, 0.0f)
};

KeyColorPicker::KeyColorPicker(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent), mDelegate(nullptr), mSelectorOffs(0), mMousePressed(false)
{
    mGradientTexture.create(widgetRect.size.x, widgetRect.size.y, LGL::Color::Transparent);
    redrawGradient();
}

void KeyColorPicker::onRedrawThis()
{
    mTexture.drawTexture(mGradientTexture, Vec2i());

    mTexture.drawRect(IntRect(Vec2i(0, mSelectorOffs - SELECTOR_SIZE / 2),
                              Vec2i(mRect.size.x, SELECTOR_SIZE)));
}

void KeyColorPicker::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mMouseOffs = localMousePos.y;
    if (mMousePressed)
    {
        mSelectorOffs = mMouseOffs;
        
        if (mDelegate != nullptr)
            mDelegate->onKeyColorChanged(calcColor(mSelectorOffs));
    }
}

void KeyColorPicker::onMouseClicked()
{
    mMousePressed = true;
    mSelectorOffs = mMouseOffs;

    if (mDelegate != nullptr)
        mDelegate->onKeyColorChanged(calcColor(mSelectorOffs));
}

void KeyColorPicker::onMouseReleased()
{
    mMousePressed = false;
}

void KeyColorPicker::redrawGradient()
{
    for (int y = 0; y < mRect.size.y; y++)
    {
        for (int x = 0; x < mRect.size.x; x++)
        {
            mGradientTexture.setPixel(x, y, calcColor(y));
        }
    }

    mGradientTexture.update();
}

LGL::Color KeyColorPicker::calcColor(int position)
{
    float sectorFloat = (float)position * (BASE_COLORS_COUNT - 1) / mRect.size.y;
    int sector = sectorFloat;

    float mixCoef = 1.0f - (sectorFloat  - sector);

    return LGL::Color(mixCoef * baseColors[sector].r + (1.0 - mixCoef) * baseColors[sector + 1].r,
                      mixCoef * baseColors[sector].g + (1.0 - mixCoef) * baseColors[sector + 1].g,
                      mixCoef * baseColors[sector].b + (1.0 - mixCoef) * baseColors[sector + 1].b);
}