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

protected:
    virtual void onRedrawThis() override;

    virtual EventResult onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier) override;

private:
    void getTextures();

protected:
    TextBoxDelegate *mDelegate;

    char mText[MAX_TEXT_BOX_LEN + 1];
    int  mCursorPos;
    int  mTextLen;

    const LGL::Texture *mTextBoxBodyTexture;
    const LGL::Texture *mTextBoxLeftTexture;
    const LGL::Texture *mTextBoxRightTexture;
};

#endif