#include <cstring>
#include "../TextureManager.hpp"
#include "../GUILogic/TextBox/TextBoxDelegate.hpp"
#include "TextBox.hpp"

const float CURSOR_BLINK_PERIOD = 1.0f;

const int TEXT_BOX_HEIGHT    = 20;
const int TEXT_BOX_SIDE_SIZE = 2;
const int TEXT_OFFSET        = 4;
const int CURSOR_WIDTH       = 1;

TextBox::TextBox(const Vec2i &textBoxPos, int textBoxSize, Widget *parent) :
    Widget(IntRect(textBoxPos, Vec2i(textBoxSize, TEXT_BOX_HEIGHT))), mDelegate(nullptr), mTextLen(0),
    mCursorTextOffset(0), mCursorOffset(0), mDisplayCursor(false)
{
    getTextures();

    memset(mText, 0, sizeof(char) * MAX_TEXT_BOX_LEN);
}

void TextBox::onUpdateThis(float elapsedTime)
{
    mBlinkTimer += elapsedTime;
    if (mBlinkTimer >= CURSOR_BLINK_PERIOD)
        mBlinkTimer -= CURSOR_BLINK_PERIOD;
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

    // Text
    mTexture.drawText(Vec2i(TEXT_OFFSET, 0), mText);

    if (mDisplayCursor && mBlinkTimer < CURSOR_BLINK_PERIOD / 2)
    {
        // Cursor
        mTexture.drawRect(IntRect(Vec2i(TEXT_OFFSET + mCursorOffset, 0), Vec2i(CURSOR_WIDTH, mRect.size.y)),
                          LGL::Color::Black);
    }
}

EventResult TextBox::onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    // 1. Check for the input

    char typedChar = LGL::toCharacter(key, modifier);
    if (typedChar != '\0')
    {
        // Insert symbol

        if (mTextLen < MAX_TEXT_BOX_LEN)
        {
            char currChar = mText[mCursorTextOffset];
            mText[mCursorTextOffset] = typedChar;

            for (int i = mCursorTextOffset + 1;; i++)
            {
                char tmp = mText[i];
                mText[i] = currChar;
                currChar = tmp;

                if (currChar == '\0')
                    break;
            }

            mCursorTextOffset++;
            mTextLen++;

            if (mDelegate != nullptr)
                mDelegate->onTextChange(mText);
        }

        recalcCursorOffset();
        mBlinkTimer = 0.0f;
        return EventResult::Handled;
    }

    // 2. Check for control sequence

    switch (key)
    {
    case LGL::KeyboardKey::Backspace:
        // Delete symbol

        if (mCursorTextOffset == 0)
            break;

        for (int i = mCursorTextOffset - 1;; i++)
        {
            mText[i] = mText[i + 1];

            if (mText[i + 1] == '\0')
                break;
        }

        mCursorTextOffset--;
        mTextLen--;

        if (mDelegate != nullptr)
            mDelegate->onTextChange(mText);
        
        break;

    case LGL::KeyboardKey::Left:
        // Move cursor left

        mCursorTextOffset--;
        if (mCursorTextOffset < 0)
            mCursorTextOffset = 0;

        break;

    case LGL::KeyboardKey::Right:
        // Move cursor right

        mCursorTextOffset++;
        if (mCursorTextOffset > mTextLen)
            mCursorTextOffset = mTextLen;

        break;

    case LGL::KeyboardKey::Home:
        // Move cursor to the beginning

        mCursorTextOffset = 0;
        break;

    case LGL::KeyboardKey::End:
        // Move cursor to the end

        mCursorTextOffset = mTextLen;
        break;

    default:
        // Not a control sequence
        return EventResult::Ignored;
    }

    recalcCursorOffset();
    mBlinkTimer = 0.0f;
    return EventResult::Handled;
}

void TextBox::onKeyboardFocusReceivedThis()
{
    mDisplayCursor = true;
    mBlinkTimer = 0.0f;
}

void TextBox::onKeyboardFocusLostThis()
{
    mDisplayCursor = false;
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

void TextBox::recalcCursorOffset()
{
    mCursorOffset = LGL::RenderTexture::calculateCharacterOffset(mText, mCursorTextOffset);
}