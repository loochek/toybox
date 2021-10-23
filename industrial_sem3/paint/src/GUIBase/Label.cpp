#include <cassert>
#include "Label.hpp"

const int LABEL_INIT_SIZE = 10;

Label::Label(Widget *parent) :
    Widget(IntRect(Vec2i(), Vec2i(LABEL_INIT_SIZE, LABEL_INIT_SIZE)), parent), mText(nullptr)
{
}

void Label::setText(const char *text)
{
    assert(text != nullptr);

    Vec2f textRectSize = LGL::RenderTarget::calculateTextBounds(text);
    mTexture.resize(textRectSize);
    mRect.size = textRectSize;

    mText = text;
}

void Label::onRedrawThis()
{
    if (mText == nullptr)
        return;

    mTexture.drawRect(mRect, LGL::Color::Yellow);
    mTexture.drawText(mRect.position, mText);
}