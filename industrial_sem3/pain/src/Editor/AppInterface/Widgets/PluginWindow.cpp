#include "PluginWindow.hpp"

PluginWindowIntl::PluginWindowIntl(const IntRect &contentRect, PluginWidgetIntl *parent) :
    Window(contentRect, parent),
    PluginWidgetIntl(IntRect(contentRect.position - Vec2i(SIDE_BORDER_SIZE, HEADER_HEIGHT),
                     contentRect.size + Vec2i(SIDE_BORDER_SIZE * 2, HEADER_HEIGHT + BOTTOM_BORDER_SIZE)), parent)
{
}

PluginWindowImpl::PluginWindowImpl(const P::WBody &body, PluginWindowIntl *widget, P::Widget *parent) :
    P::Window(body, parent), PluginWidgetImpl(body, widget, parent), P::Widget(body, parent)
{
}

void PluginWindowImpl::set_show_handler(HandlerType &handler_)
{

}

P::Window::HandlerType &PluginWindowImpl::get_show_handler()
{

}

void PluginWindowImpl::set_hide_handler(HandlerType &handler_)
{

}

P::Window::HandlerType &PluginWindowImpl::get_hide_handler()
{

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