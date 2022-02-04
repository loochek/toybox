#include "VisualSortWidget.hpp"
#include "../Sorting/VisualSortObject.hpp"

VisualSortWidget::VisualSortWidget(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent), mArray(nullptr), mHighlighIdx1(-1), mHighlighIdx2(-1)
{
}

void VisualSortWidget::onRedrawThis()
{
    mTexture.clear(LGL::Color::White);

    if (mArray == nullptr)
        return;

    // Determine max value
    int maxValue = -1;
    for (int i = 0; i < mArray->size(); i++)
    {
        maxValue = std::max(maxValue, (*mArray)[i].getValue());
    }

    float valueStep = (float)mRect.size.y / maxValue;
    float colWidth  = (float)mRect.size.x / mArray->size();

    for (int i = 0; i < mArray->size(); i++)
    {
        float height = valueStep * (*mArray)[i].getValue();
        float colYOffs = mRect.size.y - height;

        LGL::Color color = LGL::Color::Black;
        if (i == mHighlighIdx1 || i == mHighlighIdx2)
            color = LGL::Color::Red;

        mTexture.drawRect(FloatRect(Vec2f(colWidth * i, colYOffs), Vec2f(colWidth, height)), color);
    }
}