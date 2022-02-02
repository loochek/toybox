#ifndef LABEL_HPP
#define LABEL_HPP

#include "Widget.hpp"

const char MAX_LABEL_SIZE = 50;

class Label : public Widget
{
public:
    Label() = delete;

    /**
     * \param baseline Local coordinates of the initial point of the text axis
     * (average between baseline and median).
     */
    Label(const Vec2i &textAxis, Widget *parent = nullptr);

    /**
     * Sets text to a label. 
     * Note that widget rect size will be changed. 
     * Pass null pointer to clear the label
     * 
     * \param text Label text
     */
    void setText(const char *text);

    /**
     * \return Current text of the label
     */
    const char *getText() { return mText; };

protected:
    virtual void onRedrawThis() override;

private:
    Vec2i mAxis;

    char mText[MAX_LABEL_SIZE + 1];
    bool mTextPresent;
};

#endif