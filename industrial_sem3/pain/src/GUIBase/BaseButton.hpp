#ifndef BASE_BUTTON_HPP
#define BASE_BUTTON_HPP

#include "../GUILogic/ButtonDelegate.hpp"
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

    /**
     * Sets user data for the button. 
     * This value is passed to the delegate and can be used 
     * to handle multiple buttons with single delegate
     * 
     * \param userData User data
     */
    void setUserData(int userData) { this->mUserData = userData; };

protected:
    BaseButton() = delete;
    BaseButton(const IntRect &widgetRect, Widget *parent = nullptr);

    virtual ~BaseButton() {};

protected:
    ButtonDelegate *mDelegate;
    int mUserData;
};

#endif