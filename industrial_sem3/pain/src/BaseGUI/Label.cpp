#include <cassert>
#include <cstring>
#include "Label.hpp"

const Vec2i LABEL_INIT_SIZE = Vec2i(10, 10);

Label::Label(const Vec2i &textAxis, Widget *parent) :
    Widget(IntRect(textAxis, LABEL_INIT_SIZE), parent), mText(), mTextPresent(false), mAxis(textAxis)
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
        
        if (textRectSize.x < LABEL_INIT_SIZE.x)
            textRectSize.x = LABEL_INIT_SIZE.x;

        if (textRectSize.y < LABEL_INIT_SIZE.y)
            textRectSize.y = LABEL_INIT_SIZE.y;
           
        mTexture.resize(textRectSize);
        mRect = IntRect(mAxis - Vec2i(0, LGL::RenderTexture::calculateTextAxis(text)), textRectSize);

        strncpy(mText, text, MAX_LABEL_SIZE);
        mTextPresent = true;
    }
}

void Label::onRedrawThis()
{
    if (!mTextPresent)
        return;

    //mTexture.drawRect(IntRect(Vec2i(), mRect.size), LGL::Color::Yellow);
    mTexture.drawText(Vec2i(), mText);
}