#ifndef PBUTTON_DELEGATE
#define PBUTTON_DELEGATE

#include "../PluginButton.hpp"
#include "../../../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"

class PButtonDelegate : public ButtonDelegate
{
public:
    PButtonDelegate() = delete;
    PButtonDelegate(PluginButtonIntl *button) : mButton(button) {};

    virtual void onClick(uint64_t userData) override
    {
        PUPPY::Button::HandlerType &handler = mButton->getImpl()->get_handler();
        if (handler)
            handler();
    }

protected:
    PluginButtonIntl *mButton;
};

#endif