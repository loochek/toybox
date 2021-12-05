#include <cstring>
#include <cassert>
#include "PluginConfigWindow.hpp"
#include "../BaseGUI/BaseButton.hpp"
#include "../BaseGUI/Label.hpp"
#include "../BaseGUI/TextBox.hpp"
#include "../BaseGUI/Slider.hpp"
#include "Pallete.hpp"
#include "../BaseGUILogic/Window/WindowHideDelegate.hpp"
#include "../EditorLogic/PluginConfigWindow/PluginSettingsNotifier.hpp"

const int WINDOW_WIDTH   = 378;
const int LABEL_MAX_SIZE = 100;

const int VERTICAL_SPACING      = 10;
const int HORIZONTAL_SPACING    = 10;
const int LABEL_VERTICAL_ADJUST = 8;
const int TEXT_BOX_SIZE         = 250;
const int SLIDER_SIZE           = 250;

const int SLIDER_PRECISION = 100;

const Vec2i INIT_SIZE(WINDOW_WIDTH, 100);

PluginConfigWindow::PluginConfigWindow(const Vec2i &windowPos, PaintController *controller,
                                       Plugin *plugin, Widget *parent) :
    Window(IntRect(windowPos, INIT_SIZE), parent),
    mNewParamVertOffset(Window::HEADER_HEIGHT + VERTICAL_SPACING),
    mNewParamHorizOffset(Window::SIDE_BORDER_SIZE + HORIZONTAL_SPACING)
{
    mCloseButtonDelegate = new WindowHideDelegate(this);
    mCloseButton->setDelegate(mCloseButtonDelegate);

    mNotifier = new PluginSettingsNotifier(plugin);
}

PluginConfigWindow::~PluginConfigWindow()
{
    delete mCloseButtonDelegate;
    delete mNotifier;
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
    assert(structPtr != nullptr);
    if (handle == nullptr)
        return;

    TextBox *textBox = dynamic_cast<TextBox*>((Widget*)handle);
    if (textBox != nullptr)
    {
        ((PTextFieldSetting*)structPtr)->text = textBox->getText();
        return;
    }

    Slider *slider = dynamic_cast<Slider*>((Widget*)handle);
    if (slider != nullptr)
    {
        ((PSlider1dSetting*)structPtr)->frac = (float)slider->getValue() / SLIDER_PRECISION;
        return;
    }

    Pallete *pallete = dynamic_cast<Pallete*>((Widget*)handle);
    if (pallete != nullptr)
    {
        ((PColorPickerSetting*)structPtr)->color = Plugin::toPluginColor(pallete->getColor());
        return;
    }
}

void *PluginConfigWindow::addTextField(const char *name)
{
    addLabel(name);

    TextBox *textBox = new TextBox(Vec2i(mNewParamHorizOffset + LABEL_MAX_SIZE, mNewParamVertOffset),
                                   TEXT_BOX_SIZE, this);
    textBox->setDelegate(mNotifier);
    addChild(textBox);

    mNewParamVertOffset += textBox->getRect().size.y + VERTICAL_SPACING;

    resize(Vec2i(WINDOW_WIDTH, mNewParamVertOffset - Window::HEADER_HEIGHT));
    return textBox;
}

void *PluginConfigWindow::addSlider1D(const char *name)
{
    addLabel(name);

    Slider *slider = new Slider(Vec2i(mNewParamHorizOffset + LABEL_MAX_SIZE, mNewParamVertOffset),
                                SLIDER_SIZE, this);
    slider->setMaxValue(SLIDER_PRECISION);
    slider->setDelegate(mNotifier);
    addChild(slider);

    mNewParamVertOffset += slider->getRect().size.y + VERTICAL_SPACING;

    resize(Vec2i(WINDOW_WIDTH, mNewParamVertOffset - Window::HEADER_HEIGHT));
    return slider;
}

void *PluginConfigWindow::addColorPicker(const char *name)
{
    addLabel(name);

    Pallete *pallete = new Pallete(Vec2i(mNewParamHorizOffset + LABEL_MAX_SIZE, mNewParamVertOffset), this);
    pallete->setDelegate(mNotifier);
    addChild(pallete);

    mNewParamVertOffset += pallete->getRect().size.y;

    resize(Vec2i(WINDOW_WIDTH, mNewParamVertOffset - Window::HEADER_HEIGHT));
    return pallete;
}

void PluginConfigWindow::addLabel(const char *text)
{
    Label *label = new Label(Vec2i(mNewParamHorizOffset, mNewParamVertOffset + LABEL_VERTICAL_ADJUST), this);
    label->setText(text);
    addChild(label);
}