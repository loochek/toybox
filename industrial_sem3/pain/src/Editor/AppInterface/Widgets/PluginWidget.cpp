#include <cassert>
#include "../../../Utils/Logger.hpp"
#include "../../../BaseGUI/Label.hpp"
#include "../../../EditorWidgets/PaintMainWindow.hpp"
#include "../RenderTarget.hpp"
#include "PluginWidget.hpp"

PluginWidgetIntl::PluginWidgetIntl(const IntRect &widgetRect, PUPPY::Widget *impl, Widget *parent) :
    Widget(widgetRect, parent), mImpl(impl)
{
}

PluginWidgetIntl::~PluginWidgetIntl()
{
    delete mImpl;
}

EVENTS_FWD(PluginWidgetIntl, Widget)

PluginWidgetImpl::PluginWidgetImpl(const PUPPY::WBody &body, PUPPY::Widget *parent, bool create) :
    mParent(parent), mTexture(nullptr)
{
    if (create)
    {
        mWidget = new PluginWidgetIntl(fromPluginRect(body), this, translateParent(parent));
        if (parent != nullptr)
            parent->add_child(this);
    }
}

PluginWidgetImpl::PluginWidgetImpl(::Widget *widget) : mWidget(widget), mParent(nullptr), mTexture(nullptr)
{
}

void PluginWidgetImpl::set_position(const PUPPY::Vec2f &position_)
{
    mWidget->setPosition(fromPluginVec(position_));
}

void PluginWidgetImpl::set_size(const PUPPY::Vec2f &size_)
{
    mWidget->resize(fromPluginVec(size_));
}

PUPPY::WBody PluginWidgetImpl::get_body()
{
    return toPluginRect(mWidget->getRect());
}

void PluginWidgetImpl::set_body(const PUPPY::WBody &body_)
{
    mWidget->resize(fromPluginVec(body_.size));
    mWidget->setPosition(fromPluginVec(body_.position));
}

bool PluginWidgetImpl::is_active()
{
    return mWidget->isEnabled();
}

bool PluginWidgetImpl::is_inside(const PUPPY::Vec2f &pos)
{
    return IntRect(Vec2i(), mWidget->getRect().size).contains(fromPluginVec(pos));
}

bool PluginWidgetImpl::add_child(PUPPY::Widget *child)
{
    PluginWidgetImpl *childImpl = dynamic_cast<PluginWidgetImpl*>(child);
    if (childImpl == nullptr)
    {
        PluginWidgetIntl *newIntl = new PluginWidgetIntl(fromPluginRect(child->get_body()), child, nullptr);
        mWidget->addChild(newIntl);
    }
    else
        mWidget->addChild(childImpl->mWidget);

    return true;
}

void PluginWidgetImpl::set_to_delete()
{
    mWidget->scheduleForDeletion();
}

bool PluginWidgetImpl::delete_child(PUPPY::Widget *child)
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

void PluginWidgetImpl::focus()
{
    PaintMainWindow *probablyRoot = dynamic_cast<PaintMainWindow*>(mWidget->getParent());
    if (probablyRoot != nullptr)
        probablyRoot->popUp(mWidget);
}

void PluginWidgetImpl::set_caption(const char *text, size_t font_size, const PUPPY::Vec2f *pos)
{
    Label *caption = new Label(Vec2i(0, mWidget->getRect().size.y / 2), mWidget);
    caption->setText(text);
    if (pos == nullptr)
    {
        caption->setPosition(Vec2i(mWidget->getRect().size.x / 2 - caption->getRect().size.x / 2,
                                   caption->getRect().position.y));
    }
    else
        caption->setPosition(fromPluginVec(*pos));

    mWidget->addChild(caption);
}

void PluginWidgetImpl::set_base_color(const PUPPY::RGBA &color)
{
}

::Widget *PluginWidgetImpl::translateParent(PUPPY::Widget *parent)
{
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        return parentImpl->getWidget();
    }

    return nullptr;
}

bool PluginWidgetImpl::add_child(PluginWidgetImpl *child)
{
    mWidget->addChild(child->mWidget);
    return true;
}