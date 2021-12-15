#ifndef PALLETE_HPP
#define PALLETE_HPP

#include "../BaseGUI/Widget.hpp"

class ColorChangeDelegate;
class PalleteController;
class MainColorPicker;
class KeyColorPicker;
class Slider;
class Label;

class Pallete : public Widget
{
public:
    Pallete() = delete;
    Pallete(const IntRect &widgetRect, Widget *parent = nullptr);

    virtual ~Pallete();

    /**
     * Sets delegate for the pallete
     * It is triggered when selected color changed
     * 
     * \param delegate Delegate or null pointer
     */
    void setDelegate(ColorChangeDelegate *delegate) { mDelegate = delegate; }

    /**
     * \return Current selected color
     */
    LGL::Color getColor();

protected:
    virtual void onRedrawThis() override;

public:
    static const Vec2i PALLETE_SIZE;

protected:
    PalleteController *mController;
    ColorChangeDelegate *mDelegate;

    MainColorPicker *mMainColorPicker;
    KeyColorPicker  *mKeyColorPicker;
    Slider          *mOpacitySlider;
    Label           *mOpacityLabel;

    Vec2i mColorPreviewPos;

    friend class PalleteController;
};

#endif