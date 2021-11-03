#include "ColorPicker.hpp"
#include "../GUIBase/Button.hpp"
#include "../GUIElements/KeyColorPicker.hpp"
#include "../GUIElements/MainColorPicker.hpp"
#include "../GUILogic/ColorPickerDelegate.hpp"
#include "../GUILogic/ColorPickerController.hpp"

using LGL::Color;

const Vec2i ColorPicker::PICKER_SIZE(260, 220);

ColorPicker::ColorPicker(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent)
{
    //mDelegate = new ColorPickerDelegate();

    KeyColorPicker *keyColorPicker = new KeyColorPicker(IntRect(Vec2i(220, 10), Vec2i(30, 200)), this);
    MainColorPicker *mainColorPicker = new MainColorPicker(IntRect(Vec2i(10, 10), Vec2i(200, 200)), this);
    mController = new ColorPickerController(mainColorPicker, keyColorPicker);

    keyColorPicker->setDelegate(mController);

    addChild(keyColorPicker);
    addChild(mainColorPicker);
}

ColorPicker::~ColorPicker()
{
    delete mController;
}

void ColorPicker::subscribeCanvas(Canvas *canvas)
{
    //mDelegate->addCanvas(canvas);
}

void ColorPicker::unsubscribeCanvas(Canvas *canvas)
{
    //mDelegate->removeCanvas(canvas);
}