#ifndef SLIDER_HPP
#define SLIDER_HPP

#include "Widget.hpp"

class SliderSideButtonsDelegate;

class Slider : public Widget
{
public:
    Slider() = delete;
    Slider(const IntRect &widgetRect, Widget *parent = nullptr);

    virtual ~Slider();

    /**
     * Sets max value for a slider.
     * 
     * \param value New max value
     */
    void setMaxValue(int value) { mMaxValue = value; };

    /**
     * \return Slider value from range [0;MaxValue]
     */
    int getValue() { return mValue; };

protected:
    virtual void onUpdateThis(float elapsedTime);

protected:
    SliderSideButtonsDelegate *mSideButtonsDelegate;

    int mMaxValue;
    int mValue;

    friend class SliderSideButtonsDelegate;
};

#endif