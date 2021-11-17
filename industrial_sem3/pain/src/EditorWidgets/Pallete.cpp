#include "Pallete.hpp"
#include "../BaseGUI/Slider.hpp"
#include "../BaseGUI/Label.hpp"
#include "../EditorWidgets/KeyColorPicker.hpp"
#include "../EditorWidgets/MainColorPicker.hpp"
#include "../EditorLogic/Pallete/PalleteController.hpp"

using LGL::Color;

const Vec2i Pallete::PALLETE_SIZE(260, 260);

const IntRect KEY_COLOR_PICKER_RECT  = IntRect(Vec2i(220, 10), Vec2i(30, 200));
const IntRect MAIN_COLOR_PICKER_RECT = IntRect(Vec2i(10, 10), Vec2i(200, 200));
const IntRect CURR_COLOR_RECT        = IntRect(Vec2i(220, 220), Vec2i(30, 30));

const Vec2i OPACITY_SLIDER_POS  = Vec2i(10, 234);
const int   OPACITY_SLIDER_SIZE = 200;

const Vec2i OPACITY_LABEL_POS = Vec2i(10, 220);

Pallete::Pallete(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PALLETE_SIZE), parent), mDelegate(nullptr)
{
    mController = new PalleteController(this);
    
    mKeyColorPicker = new KeyColorPicker(KEY_COLOR_PICKER_RECT, this);
    mKeyColorPicker->setDelegate(mController);
    mKeyColorPicker->setUserData((int)ColorPickerType::Key);
    addChild(mKeyColorPicker);

    mMainColorPicker = new MainColorPicker(MAIN_COLOR_PICKER_RECT, this);
    mMainColorPicker->setDelegate(mController);
    mMainColorPicker->setUserData((int)ColorPickerType::Main);
    addChild(mMainColorPicker);

    mOpacityLabel = new Label(OPACITY_LABEL_POS, this);
    mOpacityLabel->setText("Opacity: 100%");
    addChild(mOpacityLabel);

    mOpacitySlider = new Slider(OPACITY_SLIDER_POS, OPACITY_SLIDER_SIZE, this);
    mOpacitySlider->setDelegate(mController);
    mOpacitySlider->setMaxValue(EXTERNAL_RGB_BASE);
    mOpacitySlider->setValue(EXTERNAL_RGB_BASE);
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