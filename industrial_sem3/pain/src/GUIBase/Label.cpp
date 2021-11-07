#include <cassert>
#include <cstring>
#include "Label.hpp"

const Vec2i LABEL_INIT_SIZE = Vec2i(10, 10);

Label::Label(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, LABEL_INIT_SIZE), parent), mText(), mTextPresent(false)
{
}

void Label::setText(const char *text)
{
    if (text == nullptr)
    {
        mTexture.resize(LABEL_INIT_SIZE);
        mTextPresent = false;
    }
    else
    {
        Vec2f textRectSize = LGL::RenderTarget::calculateTextBounds(text);
        mTexture.resize(textRectSize);
        mRect.size = textRectSize;

        strncpy(mText, text, MAX_LABEL_SIZE);
        mTextPresent = true;
    }
}

void Label::onRedrawThis()
{
    if (!mTextPresent)
        return;

    mTexture.drawText(Vec2i(), mText);
}