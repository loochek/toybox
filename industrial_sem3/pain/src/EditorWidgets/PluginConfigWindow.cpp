#include <cstring>
#include "PluginConfigWindow.hpp"
#include "../BaseGUI/BaseButton.hpp"
#include "../BaseGUI/Label.hpp"
#include "../BaseGUI/TextBox.hpp"
#include "../BaseGUI/Slider.hpp"
#include "Pallete.hpp"
#include "../EditorLogic/PalleteWindow/PalleteCloseDelegate.hpp"

const int WINDOW_WIDTH   = 378;
const int LABEL_MAX_SIZE = 100;

const int VERTICAL_SPACING      = 10;
const int HORIZONTAL_SPACING    = 10;
const int LABEL_VERTICAL_ADJUST = 8;
const int TEXT_BOX_SIZE         = 250;
const int SLIDER_SIZE           = 250;

const int SLIDER_MAX_VALUE = 100;

const Vec2i INIT_SIZE(WINDOW_WIDTH, 100);

PluginConfigWindow::PluginConfigWindow(const Vec2i &windowPos, PaintController *controller, Widget *parent) :
    Window(IntRect(windowPos, INIT_SIZE), parent),
    mNewParamVertOffset(Window::HEADER_HEIGHT + VERTICAL_SPACING),
    mNewParamHorizOffset(Window::SIDE_BORDER_SIZE + HORIZONTAL_SPACING)
{
}

PluginConfigWindow::~PluginConfigWindow()
{
    //delete mCloseButtonDelegate;
}

void *PluginConfigWindow::addParameter(PSettingType type, const char *name)
{
    if (strcmp(type, PST_TEXT_LINE) == 0)
        return addTextField(name);
    else if (strcmp(type, PST_SLIDER_1D) == 0)
        return addSlider1D(name);
    else if (strcmp(type, PST_COLOR_PICKER) == 0)
        return addColorPicker(name);

    return nullptr;
}

void PluginConfigWindow::getParameter(void *handle, void *structPtr)
{

}

void *PluginConfigWindow::addTextField(const char *name)
{
    addLabel(name);

    TextBox *textBox = new TextBox(Vec2i(mNewParamHorizOffset + LABEL_MAX_SIZE, mNewParamVertOffset),
                                   TEXT_BOX_SIZE, this);
    addChild(textBox);

    mNewParamVertOffset += textBox->getRect().size.y + VERTICAL_SPACING;

    resize(Vec2i(WINDOW_WIDTH, mNewParamVertOffset));
    return nullptr;
}

void *PluginConfigWindow::addSlider1D(const char *name)
{
    addLabel(name);

    Slider *slider = new Slider(Vec2i(mNewParamHorizOffset + LABEL_MAX_SIZE, mNewParamVertOffset),
                                SLIDER_SIZE, this);
    slider->setMaxValue(SLIDER_MAX_VALUE);
    addChild(slider);

    mNewParamVertOffset += slider->getRect().size.y + VERTICAL_SPACING;

    resize(Vec2i(WINDOW_WIDTH, mNewParamVertOffset));
    return nullptr;
}

void *PluginConfigWindow::addColorPicker(const char *name)
{
    addLabel(name);

    Pallete *pallete = new Pallete(Vec2i(mNewParamHorizOffset + LABEL_MAX_SIZE, mNewParamVertOffset), this);
    addChild(pallete);

    mNewParamVertOffset += pallete->getRect().size.y;

    resize(Vec2i(WINDOW_WIDTH, mNewParamVertOffset));
    return nullptr;
}

void PluginConfigWindow::addLabel(const char *text)
{
    Label *label = new Label(Vec2i(mNewParamHorizOffset, mNewParamVertOffset + LABEL_VERTICAL_ADJUST), this);
    label->setText(text);
    addChild(label);
}