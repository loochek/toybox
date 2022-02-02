#include <cstring>
#include <cassert>
#include "../TextureManager.hpp"
#include "../BaseGUILogic/TextBox/TextBoxDelegate.hpp"
#include "TextBox.hpp"

const float CURSOR_BLINK_PERIOD = 1.0f;

const int TEXT_BOX_HEIGHT    = 20;
const int TEXT_BOX_SIDE_SIZE = 2;
const int TEXT_SAFE_ZONE     = 4;
const int CURSOR_WIDTH       = 1;

const LGL::Color SELECTION_COLOR = LGL::Color(0.16f, 0.85f, 0.86f);

TextBox::TextBox(const Vec2i &textBoxPos, int textBoxSize, Widget *parent) :
    Widget(IntRect(textBoxPos, Vec2i(textBoxSize, TEXT_BOX_HEIGHT))), mDelegate(nullptr),
    mTextLen(0), mTextWidth(0), mActiveAreaOffset(0),
    mCursorTextOffset(0), mCursorOffset(0), mDisplayCursor(false),
    mSelectionTextOffset(-1), mSelectionOffset(-1),
    mTextTexture(Vec2i(textBoxSize, TEXT_BOX_HEIGHT))
{
    getTextures();

    memset(mText, 0, sizeof(char) * (MAX_TEXT_BOX_LEN + 1));
}

void TextBox::setText(const char *newText)
{
    strncpy(mText, newText, MAX_TEXT_BOX_LEN);
    mTextLen = strlen(mText);
    mCursorOffset = mTextLen;
    mSelectionTextOffset = -1;
    recalcActiveArea();
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

    if (mSelectionTextOffset != -1)
    {
        // Selection

        int selectionStart = std::min(mCursorOffset, mSelectionOffset);
        int selectionEnd   = std::max(mCursorOffset, mSelectionOffset);

        mTexture.drawRect(IntRect(Vec2i(TEXT_SAFE_ZONE + selectionStart - mActiveAreaOffset, 0),
                                Vec2i(selectionEnd - selectionStart, mRect.size.y)),
                          SELECTION_COLOR);
    }
    else if (mDisplayCursor && mBlinkTimer < CURSOR_BLINK_PERIOD / 2)
    {
        // Cursor
        mTexture.drawRect(IntRect(Vec2i(TEXT_SAFE_ZONE + mCursorOffset - mActiveAreaOffset, 0),
                                  Vec2i(CURSOR_WIDTH, mRect.size.y)),
                          LGL::Color::Black);
    }

    // Text
    mTextTexture.clear();
    mTextTexture.drawText(Vec2i(), mText);
    mTexture.drawRenderTexture(mTextTexture, Vec2i(TEXT_SAFE_ZONE, 0), Vec2i(mActiveAreaOffset, 0));
}

EventResult TextBox::onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    char typedChar = LGL::toCharacter(key, modifier);
    if (typedChar != '\0')
    {
        // Typeable char
        onPrintableChar(typedChar);
    }
    else
    {
        // Control sequence
        bool shiftPressed = LGL::isShiftPressed(modifier);

        switch (key)
        {
        case LGL::KeyboardKey::Delete:
            onDelete(false);
            break;

        case LGL::KeyboardKey::Backspace:
            onDelete(true);
            break;

        case LGL::KeyboardKey::Left:
            onLeftArrow(shiftPressed);
            break;

        case LGL::KeyboardKey::Right:
            onRightArrow(shiftPressed);
            break;

        case LGL::KeyboardKey::Home:
            onHome(shiftPressed);
            break;

        case LGL::KeyboardKey::End:
            onEnd(shiftPressed);
            break;

        default:
            // Not a control sequence
            return EventResult::Ignored;
        }
    }

    recalcActiveArea();
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

void TextBox::recalcActiveArea()
{
    mTextWidth = LGL::RenderTarget::calculateTextBounds(mText).x;

    mSelectionOffset = LGL::RenderTexture::calculateCharacterOffset(mText, mSelectionTextOffset);

    int oldCursorOffset = mCursorOffset;
    mCursorOffset = LGL::RenderTexture::calculateCharacterOffset(mText, mCursorTextOffset);

    if (mCursorOffset > mActiveAreaOffset + mRect.size.x - TEXT_SAFE_ZONE * 2)
    {
        mActiveAreaOffset += std::min(mCursorOffset - oldCursorOffset,
                                      mTextWidth - mRect.size.x + TEXT_SAFE_ZONE * 2);
    }
    else if (mCursorOffset < mActiveAreaOffset)
        mActiveAreaOffset = std::max(mActiveAreaOffset + mCursorOffset - oldCursorOffset, 0);
}

void TextBox::onPrintableChar(char c)
{
    if (mSelectionTextOffset != -1)
    {
        // If selection is present, delete it
        onDelete(false);
    }

    if (insertChar(mCursorTextOffset, c))
    {
        mCursorTextOffset++;

        if (mDelegate != nullptr)
            mDelegate->onTextChange(mText, mUserData);
    }
}

void TextBox::onDelete(bool backspace)
{
    if (mTextLen == 0)
        return;

    if (mSelectionTextOffset != -1)
    {
        int startOffset = std::min(mCursorTextOffset, mSelectionTextOffset);
        int endOffset   = std::max(mCursorTextOffset, mSelectionTextOffset);
        deleteSubstring(startOffset, endOffset - 1);

        mCursorTextOffset = startOffset;
        mSelectionTextOffset = -1;
    }
    else
    {
        int deletePos = mCursorTextOffset;
        if (backspace)
            deletePos--;

        if (deletePos < 0 || deletePos >= mTextLen)
            return;
            
        deleteSubstring(deletePos, deletePos);
        mCursorTextOffset = deletePos;
    }

    if (mDelegate != nullptr)
        mDelegate->onTextChange(mText, mUserData);
}

void TextBox::onLeftArrow(bool shiftPressed)
{
    if ((shiftPressed && mSelectionTextOffset != -1) ||
        (!shiftPressed && mSelectionTextOffset == -1))
    {
        // Just move cursor pos if selection state isn't changed

        mCursorTextOffset--;
        if (mCursorTextOffset < 0)
            mCursorTextOffset = 0;
            
        return;
    }

    if (shiftPressed)
    {
        // Selection must start

        if (mCursorTextOffset != 0)
        {
            mSelectionTextOffset = mCursorTextOffset;
            mCursorTextOffset--;
        }
    }
    else
    {
        // Selection must gone

        mCursorTextOffset = std::min(mCursorTextOffset, mSelectionTextOffset);
        mSelectionTextOffset = -1;
    }
}

void TextBox::onRightArrow(bool shiftPressed)
{
    if ((shiftPressed && mSelectionTextOffset != -1) ||
        (!shiftPressed && mSelectionTextOffset == -1))
    {
        // Just move cursor pos if selection state isn't changed

        mCursorTextOffset++;
        if (mCursorTextOffset > mTextLen)
            mCursorTextOffset = mTextLen;

        return;
    }

    if (shiftPressed)
    {
        // Selection must start

        if (mCursorTextOffset != mTextLen)
        {
            mSelectionTextOffset = mCursorTextOffset;
            mCursorTextOffset++;
        }
    }
    else
    {
        // Selection must gone

        mCursorTextOffset = std::max(mCursorTextOffset, mSelectionTextOffset);
        mSelectionTextOffset = -1;
    }
}

void TextBox::onHome(bool shiftPressed)
{
    if ((shiftPressed && mSelectionTextOffset != -1) ||
        (!shiftPressed && mSelectionTextOffset == -1))
    {
        // Just move cursor pos if selection state isn't changed
        mCursorTextOffset = 0;
        return;
    }

    if (shiftPressed)
    {
        // Selection must start

        if (mCursorTextOffset != 0)
        {
            mSelectionTextOffset = mCursorTextOffset;
            mCursorTextOffset = 0;
        }
    }
    else
    {
        // Selection must gone

        mCursorTextOffset = 0;
        mSelectionTextOffset = -1;
    }
}

void TextBox::onEnd(bool shiftPressed)
{
    if ((shiftPressed && mSelectionTextOffset != -1) ||
        (!shiftPressed && mSelectionTextOffset == -1))
    {
        // Just move cursor pos if selection state isn't changed
        mCursorTextOffset = mTextLen;
        return;
    }

    if (shiftPressed)
    {
        // Selection must start

        if (mCursorTextOffset != mTextLen)
        {
            mSelectionTextOffset = mCursorTextOffset;
            mCursorTextOffset = mTextLen;
        }
    }
    else
    {
        // Selection must gone

        mCursorTextOffset = mTextLen;
        mSelectionTextOffset = -1;
    }
}

bool TextBox::insertChar(int offset, char c)
{
    assert(offset <= mTextLen);

    if (mTextLen >= MAX_TEXT_BOX_LEN)
        return false;

    char currChar = mText[offset];
    mText[offset] = c;

    for (int i = offset + 1; currChar != '\0'; i++)
    {
        char tmp = mText[i];
        mText[i] = currChar;
        currChar = tmp;
    }

    mTextLen++;
    return true;
}

void TextBox::deleteSubstring(int startOffset, int endOffset)
{
    assert(0 <= startOffset && startOffset <= endOffset && endOffset < mTextLen);

    int space = endOffset - startOffset + 1;
    for (int i = startOffset ; i + space <= mTextLen; i++)
        mText[i] = mText[i + space];

    int oldTextLen = mTextLen;
    mTextLen -= endOffset - startOffset + 1;
    for (int i = mTextLen; i < oldTextLen; i++)
        mText[i] = '\0';
}