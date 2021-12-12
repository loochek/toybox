#include "WidgetFactory.hpp"
#include "Widgets/PluginWidget.hpp"
#include "Widgets/PluginWindow.hpp"

WidgetFactoryImpl::WidgetFactoryImpl(Widget *root) : mRoot(root)
{
}

P::Button *WidgetFactoryImpl::button(const P::WBody &body, P::Widget *parent) const
{
    return nullptr;
}

P::Button *WidgetFactoryImpl::button(const P::Vec2f &pos, const char *caption, P::Widget *parent) const
{
    return nullptr;
}

P::Slider *WidgetFactoryImpl::slider(P::Slider::Type type, const P::WBody &body, P::Widget *parent) const
{
    return nullptr;
}

P::TextField *WidgetFactoryImpl::text_field(const P::WBody &body, P::Widget *parent) const
{
    return nullptr;
}

P::ColorPicker *WidgetFactoryImpl::color_picker(const P::WBody &body, P::Widget *parent) const
{
    return nullptr;
}

P::Label *WidgetFactoryImpl::label(const P::Vec2f &pos, const char *text, P::Widget *parent) const
{
    return nullptr;
}

P::Window *WidgetFactoryImpl::window(const char *name, const P::WBody &body, P::Widget *parent) const
{
    PluginWidgetIntl *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = dynamic_cast<PluginWidgetIntl*>(parentImpl->mWidget->getParent());
    }

    PluginWindowIntl *widget = new PluginWindowIntl(fromPluginRect(body), parentWidget);
    PluginWindowImpl *impl   = new PluginWindowImpl(body, widget, parent);

    if (parent == nullptr)
        mRoot->addChild(widget);

    return impl;
}

P::Widget *WidgetFactoryImpl::abstract(const P::WBody &body, P::Widget *parent) const
{
    PluginWidgetIntl *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = dynamic_cast<PluginWidgetIntl*>(parentImpl->mWidget->getParent());
    }

    PluginWidgetIntl *widget = new PluginWidgetIntl(fromPluginRect(body), parentWidget);
    PluginWidgetImpl *impl   = new PluginWidgetImpl(body, widget, parent);

    return impl;
}