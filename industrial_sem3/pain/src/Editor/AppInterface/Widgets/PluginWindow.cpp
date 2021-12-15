#include "../RenderTarget.hpp"
#include "PluginWindow.hpp"
#include "../../../BaseGUI/BaseButton.hpp"
#include "../../../BaseGUILogic/Window/WindowHideDelegate.hpp"
#include "../../../EditorLogic/PaintController.hpp"

PluginWindowIntl::PluginWindowIntl(const IntRect &contentRect, PluginWindowImpl *impl,
                                   PaintController *controller, Widget *parent) :
    Window(contentRect, parent), mImpl(impl), mController(controller)
{
    mCloseButtonDelegate = new WindowHideDelegate(this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
    mController->onPluginWindowCreate(this);
}

void PluginWindowIntl::onDestroyThis()
{
    mController->onPluginWindowDestroy(this);
}

PluginWindowIntl::~PluginWindowIntl()
{
    delete mCloseButtonDelegate;
    delete mImpl;
}

PluginWindowImpl::PluginWindowImpl(const char *name, const PUPPY::WBody &body, PaintController *controller,
                                   PUPPY::Widget *parent) : 
    PluginWidgetImpl(body, false, parent), mController(controller)
{
    ::Widget *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = parentImpl->getWidget();
    }

    mWidget = new PluginWindowIntl(fromPluginRect(body), this, controller, parentWidget);
    set_name(name);

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
    mController->onPluginWindowTitleChanged(static_cast<PluginWindowIntl*>(mWidget));
    return true;
}

const char *PluginWindowImpl::get_name()
{
    return static_cast<::Window*>(mWidget)->getTitle();
}