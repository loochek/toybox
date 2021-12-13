#ifndef PTEXT_BOX_DELEGATE
#define PTEXT_BOX_DELEGATE

#include "../PluginTextBox.hpp"
#include "../../../../BaseGUILogic/TextBox/TextBoxDelegate.hpp"

class PTextBoxDelegate : public TextBoxDelegate
{
public:
    PTextBoxDelegate() = delete;
    PTextBoxDelegate(PluginTextBoxIntl *textBox) : mTextBox(textBox) {};

    virtual void onTextChange(const char *newText, uint64_t userData) override
    {
        PUPPY::TextField::HandlerType &handler = mTextBox->getImpl()->get_handler();
        if (handler)
            handler(newText);
    }

protected:
    PluginTextBoxIntl *mTextBox;
};

#endif