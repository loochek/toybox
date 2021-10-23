#ifndef BASE_BUTTON_HPP
#define BASE_BUTTON_HPP

#include "../Utils/Delegate.hpp"
#include "Widget.hpp"

class ButtonDelegate;

/**
 * Base class for a button
 */
class BaseButton : public Widget
{
public:
    /**
     * Sets a delegate for the button. 
     * Resets the delegate if null pointer is passed. 
     * Button will be responsible for its deletion. 
     * 
     * \param delegate Delegate
     */
    void setDelegate(Delegate *delegate) { this->mDelegate = delegate; };

protected:
    BaseButton() = delete;
    BaseButton(const IntRect &widgetRect, Widget *parent = nullptr);

    virtual ~BaseButton();

protected:
    Delegate *mDelegate;
};

#endif