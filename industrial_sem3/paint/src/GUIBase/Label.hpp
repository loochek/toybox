#ifndef LABEL_HPP
#define LABEL_HPP

#include "Widget.hpp"

class Label : public Widget
{
public:
    Label() = delete;
    Label(Widget *parent = nullptr);

    /**
     * Sets text to a label. 
     * Note that widget rect size will be changed
     * 
     * \param text Label text
     */
    void setText(const char *text);

protected:
    virtual void onRedrawThis() override;

private:
    const char *mText;
};

#endif