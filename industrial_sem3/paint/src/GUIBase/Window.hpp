#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Widget.hpp"

class Label;

/**
 * Base class for windows
 */
class Window : public Widget
{
protected:
    Window() = delete;
    Window(const IntRect &contentRect, Widget *parent = nullptr);

protected:
    /**
     * Sets window header title
     * 
     * \param title Title text
     */
    void setTitle(const char *title);

    virtual void onRedrawThis() override;

public:
    static const int HEADER_HEIGHT;
    static const int SIDE_BORDER_SIZE;
    static const int BOTTOM_BORDER_SIZE;

protected:
    Label *mTitle;
};

#endif