#ifndef TEXT_BOX_HPP
#define TEXT_BOX_HPP

#include "../LGL/LGL.hpp"
#include "Widget.hpp"

const int MAX_TEXT_BOX_LEN = 100;

class TextBoxDelegate;

class TextBox : public Widget
{
public:
    TextBox() = delete;
    TextBox(const Vec2i &textBoxPos, int textBoxSize, Widget *parent = nullptr);

    /**
     * Sets a delegate for the text box. 
     * Resets the delegate if null pointer is passed. 
     * 
     * \param delegate Delegate
     */
    void setDelegate(TextBoxDelegate *delegate) { this->mDelegate = delegate; };

    /**
     * Sets text for a text box
     * 
     * \param newText New text
     */
    void setText(const char *newText);

    /**
     * \return Current text
     */
    const char *getText() { return mText; };

protected:
    virtual void onUpdateThis(float elapsedTime) override;
    virtual void onRedrawThis() override;

    virtual EventResult onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier) override;
    virtual void onKeyboardFocusReceivedThis() override;
    virtual void onKeyboardFocusLostThis() override;

private:
    void getTextures();
    void recalcActiveArea();

protected:
    TextBoxDelegate *mDelegate;

    char mText[MAX_TEXT_BOX_LEN + 1];
    int  mTextLen;
    int  mTextWidth;

    // In characters
    int mCursorTextOffset;
    // In pixels
    int mCursorOffset;

    int mActiveAreaOffset;

    // In characters
    int mSelectionTextStart;
    int mSelectionTextEnd;
    // In pixels
    int mSelectionStart;
    int mSelectionEnd;

    bool mDisplayCursor;
    float mBlinkTimer;

    LGL::RenderTexture mTextTexture;

    const LGL::Texture *mTextBoxBodyTexture;
    const LGL::Texture *mTextBoxLeftTexture;
    const LGL::Texture *mTextBoxRightTexture;
};

#endif