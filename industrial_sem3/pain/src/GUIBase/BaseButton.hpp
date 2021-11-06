#ifndef BASE_BUTTON_HPP
#define BASE_BUTTON_HPP

#include "../GUILogic/BaseButton/ButtonDelegate.hpp"
#include "Widget.hpp"

/**
 * Base class for a button
 */
class BaseButton : public Widget
{
public:
    /**
     * Sets a delegate for the button. 
     * Resets the delegate if null pointer is passed. 
     * 
     * \param delegate Delegate
     */
    void setDelegate(ButtonDelegate *delegate) { this->mDelegate = delegate; };

protected:
    BaseButton() = delete;
    BaseButton(const IntRect &widgetRect, Widget *parent = nullptr);

    virtual ~BaseButton() {};

protected:
    ButtonDelegate *mDelegate;
};

#endif