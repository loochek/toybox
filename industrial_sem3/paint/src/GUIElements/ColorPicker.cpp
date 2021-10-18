#include "ColorPicker.hpp"
#include "../GUIBase/Button.hpp"
#include "../GUILogic/CanvasColorChangeDelegate.hpp"

using LGL::Color;

const Vec2i ColorPicker::PICKER_SIZE(100, 40);

const int PICKER_ROWS_COUNT     = 2;
const int PICKER_COLLUMNS_COUNT = 5;

ColorPicker::ColorPicker(const Vec2i &position, Canvas *canvas, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent), mCanvas(canvas)
{
    static const Color PICKER_COLORS[PICKER_ROWS_COUNT][PICKER_COLLUMNS_COUNT] = { 
        { Color::White, Color::Black  , Color::Red   , Color::Green, Color::Blue  },
        { Color::Cyan , Color::Magenta, Color::Yellow, Color::Pink , Color::Brown }
    };

    // Create array of color buttons

    int buttonWidth  = PICKER_SIZE.x / PICKER_COLLUMNS_COUNT;
    int buttonHeight = PICKER_SIZE.y / PICKER_ROWS_COUNT;

    for (int i = 0; i < PICKER_ROWS_COUNT; i++)
    {
        for (int j = 0; j < PICKER_COLLUMNS_COUNT; j++)
        {
            Button *button = new Button(IntRect(Vec2i(buttonWidth * j, buttonHeight * i),
                                                Vec2i(buttonWidth, buttonHeight)),
                                        this, PICKER_COLORS[i][j]);

            button->setDelegate(new CanvasColorChangeDelegate(mCanvas, PICKER_COLORS[i][j]));
            addChild(button);
        }
    }
}