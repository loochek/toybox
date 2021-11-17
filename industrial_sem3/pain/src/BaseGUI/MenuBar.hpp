#ifndef MENU_BAR_HPP
#define MENU_BAR_HPP

#include "Widget.hpp"

class ButtonDelegate;

/**
 * Menu bar of text buttons
 */
class MenuBar : public Widget
{
public:
    MenuBar() = delete;
    MenuBar(Widget *parent);

    /**
     * Adds a new text button
     * 
     * \param text Button text
     * \param delegate Button delegate. Can be a null pointer.
     * \param userData User data for a button
     */
    void addButton(const char *text, ButtonDelegate *delegate, int userData = -1);

protected:
    virtual void onRedrawThis() override;

    int mFilledWidth;

public:
    static const int MENU_BAR_HEIGHT;
};

#endif