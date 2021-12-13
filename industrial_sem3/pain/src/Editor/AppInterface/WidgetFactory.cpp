#include "WidgetFactory.hpp"
#include "Widgets/PluginWidget.hpp"
#include "Widgets/PluginWindow.hpp"
#include "Widgets/PluginButton.hpp"
#include "Widgets/PluginSlider.hpp"
#include "Widgets/PluginTextBox.hpp"
#include "Widgets/PluginColorPicker.hpp"

WidgetFactoryImpl::WidgetFactoryImpl(Widget *root) : mRoot(root)
{
}

PUPPY::Button *WidgetFactoryImpl::button(const PUPPY::WBody &body, PUPPY::Widget *parent) const
{
    return new PluginButtonImpl(body, parent);
}

PUPPY::Button *WidgetFactoryImpl::button(const PUPPY::Vec2f &pos, const char *caption, PUPPY::Widget *parent) const
{
    return new PluginButtonImpl(pos, caption, parent);
}

PUPPY::Slider *WidgetFactoryImpl::slider(PUPPY::Slider::Type type, const PUPPY::WBody &body, PUPPY::Widget *parent) const
{
    if (type == PUPPY::Slider::Type::D2)
        return nullptr;

    return new PluginSliderImpl(body, parent);
}

PUPPY::TextField *WidgetFactoryImpl::text_field(const PUPPY::WBody &body, PUPPY::Widget *parent) const
{
    return new PluginTextBoxImpl(body, parent);
}

PUPPY::ColorPicker *WidgetFactoryImpl::color_picker(const PUPPY::WBody &body, PUPPY::Widget *parent) const
{
    return new PluginColorPickerImpl(body, parent);
}

PUPPY::Label *WidgetFactoryImpl::label(const PUPPY::Vec2f &pos, const char *text, PUPPY::Widget *parent) const
{
    return nullptr;
}

PUPPY::Window *WidgetFactoryImpl::window(const char *name, const PUPPY::WBody &body, PUPPY::Widget *parent) const
{
    PluginWindowImpl *impl = new PluginWindowImpl(body, parent);
    impl->set_name(name);
    mRoot->addChild(impl->getWidget());
    return impl;
}

PUPPY::Widget *WidgetFactoryImpl::abstract(const PUPPY::WBody &body, PUPPY::Widget *parent) const
{
    return new PluginWidgetImpl(body, true, parent);
}