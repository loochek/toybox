#include "ColorPicker.hpp"
#include "../GUIBase/Button.hpp"
#include "../GUILogic/ColorPickerDelegate.hpp"

using LGL::Color;

const Vec2i ColorPicker::PICKER_SIZE(100, 40);

const Color *ColorPicker::PICKER_COLORS = nullptr;

const int PICKER_ROWS_COUNT     = 2;
const int PICKER_COLLUMNS_COUNT = 5;

ColorPicker::ColorPicker(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent)
{
    mDelegate = new ColorPickerDelegate();

    static const Color pickerColors[PICKER_ROWS_COUNT][PICKER_COLLUMNS_COUNT] = { 
        { Color::White, Color::Black  , Color::Red   , Color::Green, Color::Blue  },
        { Color::Cyan , Color::Magenta, Color::Yellow, Color::Pink , Color::Brown }
    };

    PICKER_COLORS = &pickerColors[0][0];

    // Create array of color buttons

    int buttonWidth  = PICKER_SIZE.x / PICKER_COLLUMNS_COUNT;
    int buttonHeight = PICKER_SIZE.y / PICKER_ROWS_COUNT;

    for (int i = 0; i < PICKER_ROWS_COUNT; i++)
    {
        for (int j = 0; j < PICKER_COLLUMNS_COUNT; j++)
        {
            Button *button = new Button(IntRect(Vec2i(buttonWidth * j, buttonHeight * i),
                                                Vec2i(buttonWidth, buttonHeight)),
                                        this, pickerColors[i][j]);

            button->setDelegate(mDelegate);
            button->setUserData(PICKER_COLLUMNS_COUNT * i + j);
            addChild(button);
        }
    }
}

ColorPicker::~ColorPicker()
{
    delete mDelegate;
}

void ColorPicker::subscribeCanvas(Canvas *canvas)
{
    mDelegate->addCanvas(canvas);
}

void ColorPicker::unsubscribeCanvas(Canvas *canvas)
{
    mDelegate->removeCanvas(canvas);
}