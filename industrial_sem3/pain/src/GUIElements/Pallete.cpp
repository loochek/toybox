#include "Pallete.hpp"
#include "../GUIBase/Slider.hpp"
#include "../GUIElements/KeyColorPicker.hpp"
#include "../GUIElements/MainColorPicker.hpp"
#include "../GUILogic/Pallete/PalleteController.hpp"

using LGL::Color;

const Vec2i Pallete::PALLETE_SIZE(260, 260);

const IntRect KEY_COLOR_PICKER_RECT  = IntRect(Vec2i(220, 10), Vec2i(30, 200));
const IntRect MAIN_COLOR_PICKER_RECT = IntRect(Vec2i(10, 10), Vec2i(200, 200));
const IntRect OPACITY_SLIDER_RECT    = IntRect(Vec2i(10, 220), Vec2i(200, 16));
const IntRect CURR_COLOR_RECT        = IntRect(Vec2i(220, 220), Vec2i(30, 30));

Pallete::Pallete(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PALLETE_SIZE), parent), mDelegate(nullptr)
{
    mController = new PalleteController(this);
    
    mKeyColorPicker = new KeyColorPicker(KEY_COLOR_PICKER_RECT, this);
    mKeyColorPicker->setDelegate(mController);
    mKeyColorPicker->setUserData((int)ColorPickerType::Key);

    mMainColorPicker = new MainColorPicker(MAIN_COLOR_PICKER_RECT, this);
    mMainColorPicker->setDelegate(mController);
    mMainColorPicker->setUserData((int)ColorPickerType::Main);

    mOpacitySlider = new Slider(OPACITY_SLIDER_RECT.position, OPACITY_SLIDER_RECT.size.x, this);
    mOpacitySlider->setMaxValue(EXTERNAL_RGB_BASE);
    mOpacitySlider->setValue(EXTERNAL_RGB_BASE);
    mOpacitySlider->setDelegate(mController);

    addChild(mKeyColorPicker);
    addChild(mMainColorPicker);
    addChild(mOpacitySlider);
}

Pallete::~Pallete()
{
    delete mController;
}

LGL::Color Pallete::getColor()
{
    return mMainColorPicker->getColor();
}

void Pallete::onRedrawThis()
{
    mTexture.drawRect(CURR_COLOR_RECT, getColor());
}