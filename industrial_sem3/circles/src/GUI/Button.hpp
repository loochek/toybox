#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "../Graphics.hpp"
#include "RectangularWidget.hpp"

class ButtonDelegate;

/**
 * Clickable button
 */
class Button : public RectangularWidget
{
public:
    Button() = delete;
    Button(const Rect  &rect,
           const Color &idleColor     = Color(0.0f, 1.0f, 1.0f),
           const Color &selectedColor = Color(0.94f, 0.77f, 0.41f),
           const Color &pressedColor  = Color(1.0f, 0.68f, 0.0f));

    void update(float elapsedTime) override;
    void draw(Graphics &graphics) override;

    /**
     * Sets a delegate for the button
     * Resets the delegate if null pointer is passed
     * 
     * \param delegate Delegate
     */
    void setDelegate(ButtonDelegate *delegate) { this->delegate = delegate; };

    /**
     * Sets a label for the button
     * No label will drawn if null pointer is passed
     * 
     * \param label Label text
     */
    void setLabel(const char *label) { this->label = label; };

    /**
     * Sets idle color
     * 
     * \param color Color
     */
    void setIdleColor(const Color &color) { idleColor = color; };

    /**
     * Sets idle color
     * 
     * \param color Color
     */
    void setSelectedColor(const Color &color) { selectedColor = color; };

    /**
     * Sets idle color
     * 
     * \param color Color
     */
    void setPressedColor(const Color &color) { pressedColor = color; };

private:
    Color idleColor;
    Color selectedColor;
    Color pressedColor;

    ButtonDelegate *delegate;

    const char *label;
};


class ButtonDelegate
{
public:
    virtual void onClick(Button *button) = 0;
};

#endif