#include "VisualSortWidget.hpp"
#include "../Sorting/VisualSortObject.hpp"

VisualSortWidget::VisualSortWidget(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent), mSortPicture(widgetRect.size)
{
}

void VisualSortWidget::redraw(const std::vector<VisualSortObject> &array, int highlIdx1, int highlIdx2)
{
    mSortPicture.clear();

    // Determine max value
    int maxValue = -1;
    for (int i = 0; i < array.size(); i++)
    {
        maxValue = std::max(maxValue, array[i].getValue());
    }

    float valueStep = (float)mRect.size.y / maxValue;
    float colWidth  = (float)mRect.size.x / array.size();

    for (int i = 0; i < array.size(); i++)
    {
        float height = valueStep * array[i].getValue();
        float colYOffs = mRect.size.y - height;

        LGL::Color color = LGL::Color::Black;
        if (i == highlIdx1 || i == highlIdx2)
            color = LGL::Color::Red;

        mSortPicture.drawRect(FloatRect(Vec2f(colWidth * i, colYOffs), Vec2f(colWidth, height)), color);
    }
}

void VisualSortWidget::onRedrawThis()
{
    mTexture.clear(LGL::Color::White);
    mTexture.drawRenderTexture(mSortPicture, Vec2i());
}