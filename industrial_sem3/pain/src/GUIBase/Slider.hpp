#ifndef SLIDER_HPP
#define SLIDER_HPP

#include "../LGL/LGL.hpp"
#include "Widget.hpp"

class SliderDelegate;
class SliderSideButtonsDelegate;
class SliderThumb;

class Slider : public Widget
{
public:
    Slider() = delete;
    Slider(const Vec2i &sliderPos, int sliderSize, Widget *parent = nullptr);

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
    void setMaxValue(int value);

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
    virtual void onRedrawThis() override;

private:
    void thumbMoved(int newThumbXPos);
    void getTextures();
    int calcThumbSize(int sliderSize, int maxValue);

protected:
    SliderDelegate *mDelegate;

    int mMaxValue;
    int mValue;

    SliderSideButtonsDelegate *mSideButtonsDelegate;
    SliderThumb *mThumb;

    const LGL::Texture *mSliderLeftButtonIdleTexture;
    const LGL::Texture *mSliderLeftButtonHoveredTexture;
    const LGL::Texture *mSliderLeftButtonPressedTexture;
    const LGL::Texture *mSliderRightButtonIdleTexture;
    const LGL::Texture *mSliderRightButtonHoveredTexture;
    const LGL::Texture *mSliderRightButtonPressedTexture;
    const LGL::Texture *mSliderBodyTexture;

    friend class SliderThumb;
};

#endif