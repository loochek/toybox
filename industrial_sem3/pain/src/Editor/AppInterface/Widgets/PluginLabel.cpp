#include "../../../Utils/Logger.hpp"
#include "../RenderTarget.hpp"
#include "PluginLabel.hpp"

PluginLabelIntl::PluginLabelIntl(const Vec2i &textAxis, PluginLabelImpl *impl, Widget *parent) :
    Label(textAxis, parent), mImpl(impl)
{
}

PluginLabelIntl::~PluginLabelIntl()
{
    delete mImpl;
}

PluginLabelImpl::PluginLabelImpl(const PUPPY::Vec2f &pos, const char *text, PUPPY::Widget *parent) :
    PluginWidgetImpl(PUPPY::WBody(), false, parent)
{
    ::Widget *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = parentImpl->getWidget();
    }

    ::Label *label = new PluginLabelIntl(fromPluginVec(pos), this, parentWidget);
    label->setText(text);
    mWidget = label;

    if (parent != nullptr)
        parent->add_child(this);
}

std::string_view PluginLabelImpl::get_text() const
{
    return static_cast<::Label*>(mWidget)->getText();
}

void PluginLabelImpl::set_text(std::string_view text)
{
    static_cast<::Label*>(mWidget)->setText(text.data());
}