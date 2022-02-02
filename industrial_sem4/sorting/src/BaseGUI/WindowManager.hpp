 #ifndef WINDOW_MANAGER_HPP
 #define WINDOW_MANAGER_HPP

#include "Widget.hpp"

/**
 * Container class for windows with by-click reordering
 */
class WindowManager : public Widget
{
public:
    WindowManager() = delete;
    WindowManager(const IntRect &widgetRect, Widget *parent = nullptr);

    /**
     * Puts widget above others
     * 
     * \param widget Widget to pop up
     */
    void popUp(Widget* widget);

protected:
    virtual void onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;

private:
    void popUp(std::list<Widget*>::iterator widgetIter);
};

#endif