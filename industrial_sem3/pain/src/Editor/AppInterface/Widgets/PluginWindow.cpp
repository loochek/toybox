#include "../RenderTarget.hpp"
#include "PluginWindow.hpp"

PluginWindowIntl::PluginWindowIntl(const IntRect &contentRect, PluginWindowImpl *impl, Widget *parent) :
    Window(contentRect, parent), mImpl(impl)
{
}

PluginWindowIntl::~PluginWindowIntl()
{
    delete mImpl;
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
    if (parent != nullptr)
        parent->add_child(this);
}

bool PluginWindowImpl::add_child(PUPPY::Widget *child)
{
    PUPPY::Vec2f position = child->get_body().position;
    PluginWidgetImpl::add_child(child);
    child->set_position(PUPPY::Vec2f(position.x + ::Window::SIDE_BORDER_SIZE,
                                     position.y + ::Window::HEADER_HEIGHT));
    return true;
}

PUPPY::WBody PluginWindowImpl::get_body()
{
    PUPPY::WBody body = PluginWidgetImpl::get_body();
    body.position.x -= ::Window::SIDE_BORDER_SIZE;
    body.position.y -= ::Window::HEADER_HEIGHT;
    return body;
}

void PluginWindowImpl::set_body(const PUPPY::WBody &body_)
{
    PUPPY::WBody body = PUPPY::WBody(PUPPY::Vec2f(body_.position.x + ::Window::SIDE_BORDER_SIZE,
                                                  body_.position.y + ::Window::HEADER_HEIGHT),
                                     body_.size);
    PluginWidgetImpl::set_body(body_);
}

bool PluginWindowImpl::set_name(const char *name)
{
    static_cast<::Window*>(mWidget)->setTitle(name);
    return true;
}

const char *PluginWindowImpl::get_name()
{
    return static_cast<::Window*>(mWidget)->getTitle();
}