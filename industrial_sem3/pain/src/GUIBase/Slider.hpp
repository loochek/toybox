#ifndef SLIDER_HPP
#define SLIDER_HPP

#include "Widget.hpp"

class SliderDelegate;
class SliderSideButtonsDelegate;
class SliderThumb;

class Slider : public Widget
{
public:
    Slider() = delete;
    Slider(const IntRect &widgetRect, Widget *parent = nullptr);

    virtual ~Slider();

    /**
     * Sets value for a slider. 
     * If value is more than max value, it will be cropped
     * Thumb will change it's position depending on set value
     * 
     * \param value New value
     */
    void setValue(int value);

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

    /**
     * Sets a delegate for the slider. 
     * Resets the delegate if null pointer is passed. 
     * 
     * \param delegate Delegate
     */
    void setDelegate(SliderDelegate *delegate) { this->mDelegate = delegate; };

protected:
    virtual void onUpdateThis(float elapsedTime);

private:
    void thumbMoved(int newThumbXPos);

protected:
    SliderDelegate *mDelegate;

    int mMaxValue;
    int mValue;

    SliderSideButtonsDelegate *mSideButtonsDelegate;
    SliderThumb *mThumb;

    friend class SliderThumb;
};

#endif