#include "../RenderTarget.hpp"
#include "PluginWindow.hpp"

PluginWindowIntl::PluginWindowIntl(const IntRect &contentRect, PluginWindowImpl *impl, Widget *parent) :
    Window(contentRect, parent), mImpl(impl)
{
}

PluginWindowImpl::PluginWindowImpl(const PUPPY::WBody &body, PUPPY::Widget *parent) : 
    PluginWidgetImpl(body, false, parent)
{
    ::Widget *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = parentImpl->getWidget();
    }

    mWidget = new PluginWindowIntl(fromPluginRect(body), this, parentWidget);
}

bool PluginWindowImpl::set_name(const char *name)
{
    dynamic_cast<::Window*>(mWidget)->setTitle(name);
    return true;
}

const char *PluginWindowImpl::get_name()
{
    return "aboba";
}