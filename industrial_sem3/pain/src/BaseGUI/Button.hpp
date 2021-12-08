#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "../LGL/LGL.hpp"
#include "AnimatedButton.hpp"

class Label;

/**
 * Simple button
 */
class Button : public AnimatedButton
{
public:
    Button() = delete;
    Button(const IntRect &widgetRect, Widget *parent = nullptr,
           const LGL::Color &idleColor    = LGL::Color(0.0f, 1.0f, 1.0f),
           const LGL::Color &hoveredColor = LGL::Color(0.94f, 0.77f, 0.41f),
           const LGL::Color &pressedColor = LGL::Color(1.0f, 0.68f, 0.0f));

    virtual ~Button() {};

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
    virtual void onRedrawThis() override;

protected:
    LGL::Color mIdleColor;
    LGL::Color mHoveredColor;
    LGL::Color mPressedColor;
};

#endif