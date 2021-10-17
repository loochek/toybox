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
     * Child rect must be inside parent rect. 
     * 
     * \param child Child widget
     * \return True if added, false if widget's rect doesn't fit into the parent
     */
    bool addChild(Widget *child);

    /**
     * \return Widget rect
     */
    IntRect getRect() { return mRect; };

protected:
    Widget() = delete;
    
    /**
     * \param widgetRect Widget's size and position relative to parent widget
     * \param parent Widget parent
     */
    Widget(const IntRect &widgetRect, Widget *parent = nullptr);
    virtual ~Widget();

    // Events handlers

    virtual void onUpdate(const Vec2i &parentAbsPos, float elapsedTime);
    virtual void onRedraw();

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
};

#endif