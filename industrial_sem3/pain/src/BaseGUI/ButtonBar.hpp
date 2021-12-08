#ifndef BUTTON_BAR_HPP
#define BUTTON_BAR_HPP

#include "Widget.hpp"

enum class ButtonBarLocation
{
    Top,
    Bottom
};

class BaseButton;
class ButtonDelegate;

/**
 * Menu bar of text buttons
 */
class ButtonBar : public Widget
{
public:
    ButtonBar() = delete;
    ButtonBar(ButtonBarLocation location, Widget *parent);

    /**
     * Sets color to the menu bar
     * 
     * \param color Color
     */
    void setColor(const LGL::Color &color) {mColor = color; };

    /**
     * Adds a new text button
     * 
     * \param text Button text
     * \param delegate Button delegate. Can be a null pointer.
     * \param userData User data for a button
     * \return Const button which can be passed to deleteButton
     */
    const BaseButton *addButton(const char *text, ButtonDelegate *delegate, uint64_t userData = -1);

    /**
     * Deletes the button
     * 
     * \param button Button to delete
     */
    void deleteButton(const BaseButton *button);

protected:
    virtual void onRedrawThis() override;
    void rearrangeButtons();

public:
    static const int MENU_BAR_HEIGHT;

protected:
    int mFilledWidth;
    LGL::Color mColor;
};

#endif