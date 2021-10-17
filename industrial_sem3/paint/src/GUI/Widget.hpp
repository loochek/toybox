#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <vector>
#include "../Math/Vec2.hpp"
#include "../Math/Rect.hpp"
#include "../LGL/LGL.hpp"

/**
 * Base class for GUI widgets
 */
class Widget
{
public:
    /**
     * Adds children widget. 
     * 
     * \param child Child widget
     */
    void addChild(Widget *child);

    /**
     * \return Widget rect
     */
    IntRect getRect() { return mRect; };

    /**
     * Sets new position for the widget relative to parent
     * 
     * \param newPosition New position
     */
    void setPosition(const Vec2f &newPosition) { mRect.position = newPosition; };

protected:
    Widget() = delete;
    
    /**
     * \param widgetRect Widget's size and position relative to parent widget
     * \param parent Widget parent
     */
    Widget(const IntRect &widgetRect, Widget *parent = nullptr);
    virtual ~Widget();

    // Events handlers

    /// Widget should update it's state
    virtual void onUpdate(const Vec2i &parentAbsPos, float elapsedTime);

    /// Widget should update it's render texture with actual state
    virtual void onRedraw();

    /// Called when mouse drags inside widget rect
    virtual void onMouseDrag(const Vec2i &mousePosition);

    /// Called when mouse enters widget rect
    virtual void onMouseHoverBegin(const Vec2i &mousePosition);

    /// Called when mouse is clicked inside widget rect
    virtual void onMouseClicked();

    /// Called when mouse is released inside widget rect
    virtual void onMouseReleased();

    /// Called when mouse goes out from widget rect
    virtual void onMouseHoverEnd();

    /**
     * Redraws this widget. 
     * Called by redraw event handler before children are redrawn
     */
    virtual void redrawThis() {};

protected:
    Widget *mParent;
    std::vector<Widget*> mChildren;

    /// Widget's size and position relative to parent widget
    IntRect mRect;

    /// Widget's texture
    LGL::RenderTexture mTexture;

    // To access protected methods
    friend class GUIManager;

private:
    // Data for handling mouse events

    // Can be child or nullptr
    Widget *mWidgetUnderMouse;
    bool    mMousePressed;
};

#endif