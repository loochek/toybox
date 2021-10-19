#include <cassert>
#include "Label.hpp"

Label::Label(Widget *parent) : Widget(IntRect(), parent)
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

    mTexture.drawText(Vec2f(), mText);
}