#include "../../../Utils/Logger.hpp"
#include "../RenderTarget.hpp"
#include "PluginWidget.hpp"

PluginWidgetIntl::PluginWidgetIntl(const IntRect &widgetRect, PluginWidgetIntl *parent) :
    Widget(widgetRect, parent)
{
}

PluginWidgetImpl::PluginWidgetImpl(const P::WBody &body, PluginWidgetIntl *widget, P::Widget *parent) :
    P::Widget(body, parent), mWidget(widget)
{
    texture = new RenderTargetImpl(&widget->mTexture, true);
}

bool PluginWidgetImpl::is_active()
{
    return mWidget->isEnabled();
}

bool PluginWidgetImpl::is_inside(const P::Vec2f &pos)
{
    return IntRect(Vec2i(), mWidget->getRect().size).contains(fromPluginVec(pos));
}

bool PluginWidgetImpl::add_child(P::Widget *child)
{
    PluginWidgetImpl *childImpl = dynamic_cast<PluginWidgetImpl*>(child);
    mWidget->addChild(childImpl->mWidget);
    return true;
}

void PluginWidgetImpl::set_to_delete()
{
    mWidget->scheduleForDeletion();
}

bool PluginWidgetImpl::delete_child(P::Widget *child)
{
    Logger::log(LogLevel::Warning, "Child deletion is not implemented");
    return false;
}

bool PluginWidgetImpl::delete_from_parent()
{
    Logger::log(LogLevel::Warning, "Child deletion is not implemented");
    return false;
}

void PluginWidgetImpl::hide()
{
    mWidget->scheduleForDisable();
}

void PluginWidgetImpl::show()
{
    mWidget->scheduleForEnable();
}

void PluginWidgetImpl::set_caption(const char *text, size_t font_size, const P::Vec2f &pos)
{
}

void PluginWidgetImpl::set_base_color(const P::RGBA &color)
{
}