#ifndef PALLETE_CONTROLLER_HPP
#define PALLETE_CONTROLLER_HPP

#include "../../LGL/LGL.hpp"
#include "../ColorChangeDelegate.hpp"
#include "../Slider/SliderDelegate.hpp"

class Pallete;

enum class ColorPickerType
{
    Key,
    Main
};

class PalleteController : public ColorChangeDelegate, public SliderDelegate
{
public:
    PalleteController() = delete;
    PalleteController(Pallete *pallete);

    // Color pickers callback
    virtual void onColorChange(const LGL::Color &color, int userData) override;

    // Opacity slider callback
    virtual void onValueChange(int newOpacity, int userData) override;

private:
    Pallete *mPallete;

    LGL::Color mCurrColor;
};

#endif