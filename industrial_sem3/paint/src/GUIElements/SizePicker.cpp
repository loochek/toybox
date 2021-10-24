#include "SizePicker.hpp"
#include "../GUIBase/Button.hpp"
#include "../GUILogic/SizePickerDelegate.hpp"

const Vec2i SIZE_BUTTON_SIZE(60, 40);
const int BUTTONS_COUNT = 4;

const Vec2i SizePicker::PICKER_SIZE(SIZE_BUTTON_SIZE.x, SIZE_BUTTON_SIZE.y * BUTTONS_COUNT);
const int *SizePicker::PICKER_SIZES = nullptr;

const char *buttonTexts    [BUTTONS_COUNT] = { "1", "2", "5", "10" };
const int   buttonPickSizes[BUTTONS_COUNT] = {  1,   2,   5,   10  };

SizePicker::SizePicker(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent)
{
    PICKER_SIZES = buttonPickSizes;

    mDelegate = new SizePickerDelegate();

    for (int i = 0; i < BUTTONS_COUNT; i++)
    {
        Button *button = new Button(IntRect(Vec2i(0, SIZE_BUTTON_SIZE.y * i), SIZE_BUTTON_SIZE), this);
        button->setLabel(buttonTexts[i]);
        button->setDelegate(mDelegate);
        button->setUserData(i);
        addChild(button);
    }
}

SizePicker::~SizePicker()
{
    delete mDelegate;
}

void SizePicker::subscribeCanvas(Canvas *canvas)
{
    mDelegate->addCanvas(canvas);
}

void SizePicker::unsubscribeCanvas(Canvas *canvas)
{
    mDelegate->removeCanvas(canvas);
}