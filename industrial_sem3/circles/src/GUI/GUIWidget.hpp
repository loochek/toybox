#ifndef GUI_WIDGET_HPP
#define GUI_WIDGET_HPP

#include "../Math/Vec2.hpp"
#include "../Math/Rect.hpp"

class Graphics;

/**
 * Base class for GUI widgets
 */
class GUIWidget
{
protected:
    GUIWidget() {};
    virtual ~GUIWidget() {};
    
    /**
     * Updates the widget (maybe according to the updated state)
     * virtual void onMouseHovered() = 0;
     * \param elapsedTime Time delta
     */
    virtual void update(float elapsedTime) {};

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

    virtual void onMouseHoverBegin() {};
    virtual void onMouseHover() {};
    virtual void onMouseHoverEnd() {};

    virtual void onMouseClicked() {};
    virtual void onMousePressed() {};
    virtual void onMouseReleased() {};

    // to access protected methods
    friend class GUIManager;
    // to send events to widgets
    friend class MouseManager;
};

#endif