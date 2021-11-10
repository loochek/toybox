#include "KeyColorPicker.hpp"
#include "../GUILogic/ColorChangeDelegate.hpp"

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
    mGradientImage.create(widgetRect.size.x, widgetRect.size.y, LGL::Color::Transparent);
    redrawGradient();
}

void KeyColorPicker::onRedrawThis()
{
    mTexture.drawTexture(mGradientTexture, Vec2i());

    mTexture.drawRect(IntRect(Vec2i(0, mSelectorOffs - SELECTOR_SIZE / 2),
                              Vec2i(mRect.size.x, SELECTOR_SIZE)));
}

void KeyColorPicker::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mMouseOffs = localMousePos.y;
}

void KeyColorPicker::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mMouseOffs = localMousePos.y;
    if (mMousePressed)
    {
        mSelectorOffs = mMouseOffs;
        
        if (mDelegate != nullptr)
            mDelegate->onColorChange(calcColor(mSelectorOffs), mUserData);
    }
}

void KeyColorPicker::onMouseClicked()
{
    mMousePressed = true;
    mSelectorOffs = mMouseOffs;

    if (mDelegate != nullptr)
        mDelegate->onColorChange(calcColor(mSelectorOffs), mUserData);
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
            mGradientImage.setPixel(x, y, calcColor(y));
        }
    }

    mGradientTexture.loadFromImage(mGradientImage);
}

LGL::Color KeyColorPicker::calcColor(int offset)
{
    if (offset >= mRect.size.y)
        offset = mRect.size.y - 1;
    
    float sectorFloat = (float)offset * (BASE_COLORS_COUNT - 1) / mRect.size.y;
    int sector = sectorFloat;

    float mixCoef = 1.0f - (sectorFloat  - sector);

    return LGL::Color(mixCoef * baseColors[sector].r + (1.0 - mixCoef) * baseColors[sector + 1].r,
                      mixCoef * baseColors[sector].g + (1.0 - mixCoef) * baseColors[sector + 1].g,
                      mixCoef * baseColors[sector].b + (1.0 - mixCoef) * baseColors[sector + 1].b);
}