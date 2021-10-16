#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <vector>
#include "../Math/Vec2.hpp"
#include "../Math/Rect.hpp"

class Graphics;

/**
 * Base class for GUI widgets
 */
class Widget
{
protected:
    Widget(const IntRect &widgetRect, Widget *parent = nullptr);
    virtual ~Widget() {};
    
    /**
     * Updates the widget
     * 
     * \param elapsedTime Time delta
     */
    virtual void update(float elapsedTime);

    /**
     * Updates child widgets
     * 
     * \param elapsedTime Time delta
     */
    virtual void updateChildren(float elapsedTime);

    /**
     * Draws the widget
     * 
     * \param graphics Graphics context
     */
    virtual void draw(Graphics &graphics);

    /**
     * Draws child widgets
     * 
     * \param elapsedTime Time delta
     */
    virtual void drawChildren(Graphics &graphics);

    /**
     * Adds children widget. 
     * Child rect must be inside parent rect. 
     * 
     * \param child Child widget
     */
    void addChild(Widget *child);

    // /**
    //  * Checks point to be inside widget shape. 
    //  * This is the additional check
    //  * 
    //  * \param point Point to test
    //  */
    // virtual bool testShapePoint(const Vec2f &point) { return true };

    /**
     * \return Widget rect
     */
    IntRect getRect() { return mRect; };

protected:
    Widget *mParent;
    std::vector<Widget*> mChildren;

    /// Widget's size and position relative to parent widget
    IntRect mRect;
};

#endif