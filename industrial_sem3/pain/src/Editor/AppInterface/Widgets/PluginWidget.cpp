#include "../../../Utils/Logger.hpp"
#include "../../../BaseGUI/Label.hpp"
#include "../RenderTarget.hpp"
#include "PluginWidget.hpp"

PluginWidgetIntl::PluginWidgetIntl(const IntRect &widgetRect, PluginWidgetImpl *impl, Widget *parent) :
    Widget(widgetRect, parent), mImpl(impl)
{
}

PluginWidgetImpl::PluginWidgetImpl(const PUPPY::WBody &body, bool spawn, PUPPY::Widget *parent)
{
    if (spawn)
    {
        ::Widget *parentWidget = nullptr;
        if (parent != nullptr)
        {
            PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
            parentWidget = parentImpl->mWidget;
        }

        mWidget = new PluginWidgetIntl(fromPluginRect(body), this, parentWidget);
        if (parent != nullptr)
            parent->add_child(this);
    }
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

PUPPY::Widget *PluginWidgetImpl::get_parent() const
{
    return dynamic_cast<PluginWidgetIntl*>(mWidget->getParent())->getImpl();
}

void PluginWidgetImpl::set_parent(PUPPY::Widget *parent_)
{
    mWidget->setParent(dynamic_cast<PluginWidgetImpl*>(parent_)->mWidget);
}

PUPPY::RenderTarget *PluginWidgetImpl::get_texture()
{
    return new RenderTargetImpl(mWidget->getTexture(), true);
}

void PluginWidgetImpl::set_texture(PUPPY::RenderTarget *texture_)
{
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