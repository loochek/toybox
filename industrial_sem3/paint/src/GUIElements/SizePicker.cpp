#include "SizePicker.hpp"
#include "../GUIBase/Button.hpp"
#include "../GUILogic/CanvasSizeChangeDelegate.hpp"

const int BUTTONS_COUNT = 4;

const Vec2i SIZE_BUTTON_SIZE(60, 40);

const Vec2i SizePicker::PICKER_SIZE(SIZE_BUTTON_SIZE.x, SIZE_BUTTON_SIZE.y * BUTTONS_COUNT);

const char *buttonTexts    [BUTTONS_COUNT] = { "1", "2", "5", "10" };
const int   buttonPickSizes[BUTTONS_COUNT] = {  1,   2,   5,   10  };

SizePicker::SizePicker(const Vec2i &position, Canvas *canvas, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent)
{
    for (int i = 0; i < BUTTONS_COUNT; i++)
    {
        Button *button = new Button(IntRect(Vec2i(0, SIZE_BUTTON_SIZE.y * i), SIZE_BUTTON_SIZE), this);
        button->setLabel(buttonTexts[i]);
        button->setDelegate(new CanvasSizeChangeDelegate(canvas, buttonPickSizes[i]));
        addChild(button);
    }
}