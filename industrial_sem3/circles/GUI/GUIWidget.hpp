#ifndef GUI_WIDGET_HPP
#define GUI_WIDGET_HPP

#include "../Math/Vec2.hpp"
#include "../Math/Rect.hpp"

class Graphics;

enum class WidgetState
{
    Idle,
    Selected,
    Clicked,
    Pressed,
    Released
};

/**
 * Base class for GUI widgets
 */
class GUIWidget
{
protected:
    GUIWidget();
    virtual ~GUIWidget();
    
    /**
     * Updates the widget (maybe according to the updated state)
     */
    virtual void update();

    /**
     * Draws the widget
     * 
     * \param graphics Graphics context
     */
    virtual void draw(Graphics &graphics) = 0;

    /**
     * Checks point to be inside widget shape
     * 
     * \param point Point
     */
    virtual bool testShape(const Vec2f &point) = 0;

protected:
    // to access protected methods and widget state
    friend class GUIManager;

    WidgetState state;
};

#endif