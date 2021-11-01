#include "SizePicker.hpp"
#include "../GUIBase/Canvas.hpp"
#include "../GUIBase/Slider.hpp"
#include "../GUILogic/SizePickerSliderDelegate.hpp"

const Vec2i SizePicker::PICKER_SIZE = Vec2i(150, 20);

SizePicker::SizePicker(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent)
{
    mSliderDelegate = new SizePickerSliderDelegate(this);

    Slider *slider = new Slider(IntRect(Vec2i(), PICKER_SIZE), this);
    slider->setMaxValue(20);
    slider->setDelegate(mSliderDelegate);
    addChild(slider);
}

SizePicker::~SizePicker()
{
    delete mSliderDelegate;
}

void SizePicker::onSizeChanged(int newSize)
{
    for (Canvas *canvas : mCanvases)
        canvas->setPenSize(newSize);
}

void SizePicker::subscribeCanvas(Canvas *canvas)
{
    mCanvases.insert(canvas);
}

void SizePicker::unsubscribeCanvas(Canvas *canvas)
{
    mCanvases.erase(canvas);
}