#include "SizePicker.hpp"
#include "../GUIBase/Canvas.hpp"
#include "../GUIBase/Slider.hpp"
#include "../GUILogic/SizePicker/SizePickerSliderDelegate.hpp"

const int MAX_SIZE = 20;

const Vec2i SizePicker::PICKER_SIZE = Vec2i(150, 16);

SizePicker::SizePicker(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent)
{
    mSliderDelegate = new SizePickerSliderDelegate(this);

    Slider *slider = new Slider(Vec2i(), PICKER_SIZE.x, this);
    slider->setMaxValue(MAX_SIZE);
    slider->setDelegate(mSliderDelegate);
    addChild(slider);
}

SizePicker::~SizePicker()
{
    delete mSliderDelegate;
}