#include "../RenderTarget.hpp"
#include "Logic/PColorPickerDelegate.hpp"
#include "PluginColorPicker.hpp"

PluginColorPickerIntl::PluginColorPickerIntl(const IntRect &widgetRect, PUPPY::ColorPicker *impl,
                                             Widget *parent) :
    Pallete(widgetRect, parent), mImpl(impl)
{
    mDelegate = new PColorPickerDelegate(this);
    setDelegate(mDelegate);
}

PluginColorPickerIntl::~PluginColorPickerIntl()
{
    delete mDelegate;
    delete mImpl;
}

EVENTS_FWD(PluginColorPickerIntl, Pallete)

PluginColorPickerImpl::PluginColorPickerImpl(const PUPPY::WBody &body, PUPPY::Widget *parent) : 
    PluginWidgetImpl(body, parent, false)
{
    mWidget = new PluginColorPickerIntl(fromPluginRect(body), this, translateParent(parent));
    if (parent != nullptr)
        parent->add_child(this);
}

PUPPY::RGBA PluginColorPickerImpl::get_color()
{
    return toPluginColor((static_cast<Pallete*>(mWidget))->getColor());
}

void PluginColorPickerImpl::set_color(PUPPY::RGBA color)
{
}