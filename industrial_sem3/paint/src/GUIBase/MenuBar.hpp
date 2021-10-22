#ifndef MENU_BAR_HPP
#define MENU_BAR_HPP

#include "Widget.hpp"

class Delegate;

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
     * \param delegate Button delegate. Can be a null pointer. Menu bar will be responsible for its deletion
     */
    void addButton(const char *text, Delegate *delegate);

protected:
    virtual void onRedrawThis() override;

    int mFilledWidth;

public:
    static const int MENU_BAR_HEIGHT;
};

#endif