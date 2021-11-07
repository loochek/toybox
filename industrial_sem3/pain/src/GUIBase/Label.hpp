#ifndef LABEL_HPP
#define LABEL_HPP

#include "Widget.hpp"

const char MAX_LABEL_SIZE = 50;

class Label : public Widget
{
public:
    Label() = delete;
    Label(const Vec2i &position, Widget *parent = nullptr);

    /**
     * Sets text to a label. 
     * Note that widget rect size will be changed
     * Pass null pointer to clear the label
     * 
     * \param text Label text
     */
    void setText(const char *text);

protected:
    virtual void onRedrawThis() override;

private:
    char mText[MAX_LABEL_SIZE + 1];
    bool mTextPresent;
};

#endif