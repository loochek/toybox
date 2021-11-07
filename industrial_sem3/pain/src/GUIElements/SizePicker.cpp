#include "SizePicker.hpp"
#include "../GUIBase/Label.hpp"
#include "../GUIBase/Slider.hpp"
#include "../GUILogic/SizePicker/SizePickerSliderDelegate.hpp"

const Vec2i SizePicker::PICKER_SIZE = Vec2i(190, 60);

const Vec2i BRUSH_PREVIEW_POS = Vec2i(160, 30);

const Vec2i LABEL_POS = Vec2i(10, 10);

const Vec2i SLIDER_POS = Vec2i(10, 40);
const int   SLIDER_SIZE = 120;

const int INITIAL_SIZE = 2;
const int MAX_SIZE = 20;

SizePicker::SizePicker(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent), mDelegate(nullptr), mPreviewBrushSize(INITIAL_SIZE)
{
    mSliderDelegate = new SizePickerSliderDelegate(this);
    
    mLabel = new Label(LABEL_POS, this);
    addChild(mLabel);

    Slider *slider = new Slider(SLIDER_POS, SLIDER_SIZE, this);
    slider->setDelegate(mSliderDelegate);
    slider->setMaxValue(MAX_SIZE - 1);
    slider->setValue(INITIAL_SIZE - 1);
    
    addChild(slider);
}

SizePicker::~SizePicker()
{
    delete mSliderDelegate;
}

void SizePicker::onRedrawThis()
{
    mTexture.drawCircle(BRUSH_PREVIEW_POS, mPreviewBrushSize, LGL::Color::Black);
}