#include "../RenderTarget.hpp"
#include "Logic/PButtonDelegate.hpp"
#include "PluginButton.hpp"

const PUPPY::Vec2f DEFAULT_SIZE(100, 50);

PluginButtonIntl::PluginButtonIntl(const IntRect &widgetRect, PluginButtonImpl *impl, Widget *parent) :
    Button(widgetRect, parent), mImpl(impl)
{
    mDelegate = new PButtonDelegate(this);
    setDelegate(mDelegate);
}

PluginButtonIntl::~PluginButtonIntl()
{
    delete mDelegate;
    delete mImpl;
}

PluginButtonImpl::PluginButtonImpl(const PUPPY::WBody &body, PUPPY::Widget *parent) : 
    PluginWidgetImpl(body, false, parent)
{
    ::Widget *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = parentImpl->getWidget();
    }

    mWidget = new PluginButtonIntl(fromPluginRect(body), this, parentWidget);
    if (parent != nullptr)
        parent->add_child(this);
}

PluginButtonImpl::PluginButtonImpl(const PUPPY::WBody &body, const char *label, PUPPY::Widget *parent) : 
    PluginWidgetImpl(body, false, parent)
{
    ::Widget *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = parentImpl->getWidget();
    }

    ::Button *button = new PluginButtonIntl(fromPluginRect(body), this, parentWidget);
    button->setLabel(label);
    mWidget = button;
}