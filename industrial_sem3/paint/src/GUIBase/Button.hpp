#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "../LGL/LGL.hpp"
#include "../Utils/Delegate.hpp"
#include "Widget.hpp"
#include "Label.hpp"

class ButtonDelegate;

/**
 * Clickable button
 */
class Button : public Widget
{
public:
    Button() = delete;
    Button(const IntRect &widgetRect, Widget *parent = nullptr,
           const LGL::Color &idleColor    = LGL::Color(0.0f, 1.0f, 1.0f),
           const LGL::Color &hoveredColor = LGL::Color(0.94f, 0.77f, 0.41f),
           const LGL::Color &pressedColor = LGL::Color(1.0f, 0.68f, 0.0f));

    virtual ~Button();

    /**
     * Sets a delegate for the button. 
     * Resets the delegate if null pointer is passed. 
     * Button will be responsible for its deletion. 
     * 
     * \param delegate Delegate
     */
    void setDelegate(Delegate *delegate) { this->mDelegate = delegate; };

    /**
     * Sets a label for the button. 
     * No label will drawn if null pointer is passed
     * 
     * \param label Label text
     */
    void setLabel(const char *label);

    /**
     * Sets idle color
     * 
     * \param color Color
     */
    void setIdleColor(const LGL::Color &color) { mIdleColor = color; };

    /**
     * Sets hovered color
     * 
     * \param color Color
     */
    void setHoveredColor(const LGL::Color &color) { mHoveredColor = color; };

    /**
     * Sets pressed color
     * 
     * \param color Color
     */
    void setPressedColor(const LGL::Color &color) { mPressedColor = color; };

protected:
    virtual void redrawThis() override;
    
    // Button overrides mouse event handlers to prevent children from receiving them

    virtual void onMouseMove(const Vec2i &mousePosition) override {};
    virtual void onMouseHoverBegin(const Vec2i &mousePosition) override;
    virtual void onMouseClicked() override;
    virtual void onMouseReleased() override;
    virtual void onMouseHoverEnd() override;

private:
    LGL::Color mCurrColor;

    LGL::Color mIdleColor;
    LGL::Color mHoveredColor;
    LGL::Color mPressedColor;

    Delegate *mDelegate;

    Label *mLabel;
};

#endif