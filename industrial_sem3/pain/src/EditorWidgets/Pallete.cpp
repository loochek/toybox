#include "Pallete.hpp"
#include "../BaseGUI/Slider.hpp"
#include "../BaseGUI/Label.hpp"
#include "../EditorWidgets/KeyColorPicker.hpp"
#include "../EditorWidgets/MainColorPicker.hpp"
#include "../EditorLogic/Pallete/PalleteController.hpp"

using LGL::Color;

const int SPACE_SIZE = 10;
const int KEY_COLOR_PICKER_WIDTH = 30;
const int COLOR_PREVIEW_SIZE = 30;

Pallete::Pallete(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent), mDelegate(nullptr)
{
    // Calculate rects of child widgets

    Vec2i mainColorPickerSize = widgetRect.size - Vec2i(3 * SPACE_SIZE + KEY_COLOR_PICKER_WIDTH,
                                                        3 * SPACE_SIZE + COLOR_PREVIEW_SIZE);

    Vec2i keyColorPickerSize = Vec2i(KEY_COLOR_PICKER_WIDTH, mainColorPickerSize.y);
    Vec2i keyColorPickerPos  = Vec2i(2 * SPACE_SIZE + mainColorPickerSize.x, SPACE_SIZE);

    mColorPreviewPos = widgetRect.size - Vec2i(SPACE_SIZE + COLOR_PREVIEW_SIZE,
                                               SPACE_SIZE + COLOR_PREVIEW_SIZE);

    Vec2i sliderPos = Vec2i(SPACE_SIZE, widgetRect.size.y - SPACE_SIZE - Slider::SLIDER_HEIGHT);

    mController = new PalleteController(this);

    mKeyColorPicker = new KeyColorPicker(IntRect(keyColorPickerPos, keyColorPickerSize), this);
    mKeyColorPicker->setDelegate(mController);
    mKeyColorPicker->setUserData((int)ColorPickerType::Key);
    addChild(mKeyColorPicker);

    mMainColorPicker = new MainColorPicker(IntRect(Vec2i(SPACE_SIZE, SPACE_SIZE), mainColorPickerSize), this);
    mMainColorPicker->setDelegate(mController);
    mMainColorPicker->setUserData((int)ColorPickerType::Main);
    addChild(mMainColorPicker);

    mOpacityLabel = new Label(sliderPos - Vec2i(0, SPACE_SIZE), this);
    mOpacityLabel->setText("Opacity: 100%");
    addChild(mOpacityLabel);

    mOpacitySlider = new Slider(sliderPos, mainColorPickerSize.x, this);
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
    mTexture.drawRect(IntRect(mColorPreviewPos, Vec2i(COLOR_PREVIEW_SIZE, COLOR_PREVIEW_SIZE)), getColor());
}