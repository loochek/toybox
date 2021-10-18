#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <vector>
#include "../Utils/Vec2.hpp"
#include "../Utils/Rect.hpp"
#include "../LGL/LGL.hpp"

// TODO: implement event status system

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
    void setPosition(const Vec2i &newPosition) { mRect.position = newPosition; };

    /**
     * Moves widget
     * 
     * \param offset Position delta
     */
    void move(const Vec2i &offset) { mRect.position += offset; };

protected:
    Widget() = delete;
    
    /**
     * \param widgetRect Widget's size and position relative to parent widget
     * \param parent Widget parent
     */
    Widget(const IntRect &widgetRect, Widget *parent = nullptr);
    virtual ~Widget();

    /**
     * Event handlers
     * Call "*This" functions and/or dispatches events to children
     */

    /// Widget should update it's state
    virtual void onUpdate(const Vec2i &parentAbsPos, float elapsedTime);

    /// Widget should update it's render texture with actual state
    virtual void onRedraw();

    /// Called when mouse moves inside widget rect
    virtual void onMouseMove(const Vec2i &mousePosition);

    /// Called when mouse enters widget rect
    virtual void onMouseHoverBegin(const Vec2i &mousePosition);

    /// Called when mouse is clicked inside widget rect
    virtual void onMouseClicked();

    /// Called when mouse is released inside widget rect
    virtual void onMouseReleased();

    /// Called when mouse goes out from widget rect
    virtual void onMouseHoverEnd();

    /**
     * Functions which called by event handlers with/instead of dispatching event to children
     */

    virtual void redrawThis() {};
    virtual void updateThis(const Vec2i &parentAbsPos, float elapsedTime) {};

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