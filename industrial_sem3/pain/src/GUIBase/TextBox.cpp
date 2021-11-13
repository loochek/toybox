#include <cstring>
#include "../TextureManager.hpp"
#include "TextBox.hpp"

const int TEXT_BOX_HEIGHT    = 20;
const int TEXT_BOX_SIDE_SIZE = 2;

TextBox::TextBox(const Vec2i &textBoxPos, int textBoxSize, Widget *parent) :
    Widget(IntRect(textBoxPos, Vec2i(textBoxSize, TEXT_BOX_HEIGHT))), mCursorPos(0), mTextLen(0)
{
    getTextures();

    memset(mText, 0, sizeof(char) * MAX_TEXT_BOX_LEN);
}

void TextBox::onRedrawThis()
{
    // Left
    mTexture.drawTexture(*mTextBoxLeftTexture, Vec2i());
    // Right
    mTexture.drawTexture(*mTextBoxRightTexture, Vec2i(mRect.size.x - TEXT_BOX_SIDE_SIZE, 0));
    // Body
    mTexture.drawTexture(*mTextBoxBodyTexture,
                         Vec2i(TEXT_BOX_SIDE_SIZE, 0),
                         IntRect(Vec2i(), Vec2i(mRect.size.x - 2 * TEXT_BOX_SIDE_SIZE, TEXT_BOX_HEIGHT)));

    mTexture.drawText(Vec2i(), mText);
    printf("%s\n", mText);
}

EventResult TextBox::onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    char typedChar = LGL::toCharacter(key, modifier);
    if (typedChar != '\0')
    {
        // Insert symbol

        char currChar = mText[mCursorPos];
        mText[mCursorPos] = typedChar;

        for (int i = mCursorPos + 1;; i++)
        {
            char tmp = mText[i];
            mText[i] = currChar;
            currChar = tmp;

            if (currChar == '\0')
                break;
        }

        mCursorPos++;
        mTextLen++;
        return EventResult::Handled;
    }

    if (key == LGL::KeyboardKey::Backspace)
    {
        // Delete symbol

        if (mCursorPos == 0)
            return EventResult::Handled;

        for (int i = mCursorPos - 1;; i++)
        {
            mText[i] = mText[i + 1];

            if (mText[i + 1] == '\0')
                break;
        }

        mCursorPos--;
        mTextLen--;
        return EventResult::Handled;
    }

    if (key == LGL::KeyboardKey::Left)
    {
        // Move cursor left

        mCursorPos--;
        if (mCursorPos < 0)
            mCursorPos = 0;

        return EventResult::Handled;
    }

    if (key == LGL::KeyboardKey::Right)
    {
        // Move cursor right

        mCursorPos++;
        if (mCursorPos > mTextLen)
            mCursorPos = mTextLen;

        return EventResult::Handled;
    }

    return EventResult::Ignored;
}

void TextBox::getTextures()
{
    mTextBoxBodyTexture  = TextureManager::getInstance()->getTexture("text_box_body");
    mTextBoxLeftTexture  = TextureManager::getInstance()->getTexture("text_box_left");
    mTextBoxRightTexture = TextureManager::getInstance()->getTexture("text_box_right");

    if (mTextBoxBodyTexture == nullptr ||
        mTextBoxLeftTexture == nullptr ||
        mTextBoxRightTexture == nullptr)
    {
        throw std::runtime_error("Text box textures are not loaded");
    }
}
