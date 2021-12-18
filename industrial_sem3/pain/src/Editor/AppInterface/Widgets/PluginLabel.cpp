#include "../../../Utils/Logger.hpp"
#include "../RenderTarget.hpp"
#include "PluginLabel.hpp"

const int LABEL_HEIGHT_OFFS = 5;

PluginLabelIntl::PluginLabelIntl(const Vec2i &textAxis, PUPPY::Label *impl, Widget *parent) :
    Label(textAxis, parent), mImpl(impl)
{
}

PluginLabelIntl::~PluginLabelIntl()
{
    delete mImpl;
}

EVENTS_FWD(PluginLabelIntl, Label)

PluginLabelImpl::PluginLabelImpl(const PUPPY::Vec2f &pos, const char *text, PUPPY::Widget *parent) :
    PluginWidgetImpl(PUPPY::WBody(), parent, false)
{
    ::Label *label = new PluginLabelIntl(fromPluginVec(pos) + Vec2i(0, LABEL_HEIGHT_OFFS), this,
                                         translateParent(parent));
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