#include "../RenderTarget.hpp"
#include "Logic/PColorPickerDelegate.hpp"
#include "PluginColorPicker.hpp"

PluginColorPickerIntl::PluginColorPickerIntl(const Vec2i &position, PluginColorPickerImpl *impl, Widget *parent) :
    Pallete(position, parent), mImpl(impl)
{
    mDelegate = new PColorPickerDelegate(this);
    setDelegate(mDelegate);
}

PluginColorPickerIntl::~PluginColorPickerIntl()
{
    delete mDelegate;
}

PluginColorPickerImpl::PluginColorPickerImpl(const PUPPY::WBody &body, PUPPY::Widget *parent) : 
    PluginWidgetImpl(body, false, parent)
{
    ::Widget *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = parentImpl->getWidget();
    }

    mWidget = new PluginColorPickerIntl(fromPluginVec(body.position), this, parentWidget);
}

PUPPY::RGBA PluginColorPickerImpl::get_color()
{
    return toPluginColor(((Pallete*)mWidget)->getColor());
}

void PluginColorPickerImpl::set_color(PUPPY::RGBA color)
{
}