#ifndef WINDOW_HEADER_HPP
#define WINDOW_HEADER_HPP

#include "Widget.hpp"
#include "Label.hpp"

class Window;

class WindowHeader : public Widget
{
public:
    WindowHeader() = delete;
    WindowHeader(Window *parent);

    /**
     * Sets window header title
     * 
     * \param title Title text
     */
    void setTitle(const char *title);

protected:
    virtual void redrawThis() override;

public:
    static const int HEADER_HEIGHT;

protected:
    Label *mTitle;
};

#endif